/* Copyright 2008,2009,2010,2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "KGpgGroupNode.h"

#include "KGpgGroupMemberNode.h"
#include "KGpgRootNode.h"
//#include "kgpgsettings.h"

#include <QDebug>
//#include <KLocale>
#include <QFile>
#include <QStringList>
#include <QTextStream>

class KGpgGroupNodePrivate {
public:
	KGpgGroupNodePrivate(const QString &name);

	QString m_name;

	/**
	 * @brief find the line that defines this group in the configuration
	 * @param conffile file object (will be initialized)
	 * @param stream text stream (will be initialized and connected to conffile)
	 * @param lines the lines found in conffile (will be filled)
	 * @return the index in lines of the entry defining this group
	 * @retval -1 no entry defining this group was found
	 *
	 * stream will be positioned at the beginning.
	 */
	int findGroupEntry(QFile &conffile, QTextStream &stream, QStringList &lines);

	static const QRegExp &groupPattern();
	static const QString &groupTag();
};

KGpgGroupNodePrivate::KGpgGroupNodePrivate(const QString &name)
	: m_name(name)
{
}

int
KGpgGroupNodePrivate::findGroupEntry(QFile &conffile, QTextStream &stream, QStringList &lines)
{
    //conffile.setFileName(KGpgSettings::gpgConfigPath());
    conffile.setFileName("");

	if (!conffile.exists())
		return -1;

	if (!conffile.open(QIODevice::ReadWrite))
		return -1;

	stream.setDevice(&conffile);
	int index = -1;
	int i = -1;

	while (!stream.atEnd()) {
		const QString rawLine = stream.readLine();
		i++;
		QString parsedLine = rawLine.simplified().section(QLatin1Char('#'), 0, 0);

		if (groupPattern().exactMatch(parsedLine)) {
			// remove "group "
			parsedLine = parsedLine.remove(0, 6);
			if (parsedLine.startsWith(m_name)) {
				if (parsedLine.mid(m_name.length()).simplified().startsWith(QLatin1Char('='))) {
					if (index >= 0) {
						// multiple definitions of the same group, drop the second one
						continue;
					} else {
						index = i;
					}
				}
			}
		}

		lines << rawLine;
	}

	stream.seek(0);

	return index;
}

const QRegExp &
KGpgGroupNodePrivate::groupPattern()
{
	static const QRegExp groupre(QLatin1String("^group [^ ]+ ?= ?([0-9a-fA-F]{8,} ?)*$"));

	return groupre;
}

const QString &
KGpgGroupNodePrivate::groupTag()
{
	static const QString grouptag(QLatin1String("group "));

	return grouptag;
}

KGpgGroupNode::KGpgGroupNode(KGpgRootNode *parent, const QString &name, const QStringList &members)
	: KGpgExpandableNode(parent),
	d_ptr(new KGpgGroupNodePrivate(name))
{
	foreach (const QString &id, members)
		new KGpgGroupMemberNode(this, id);

	parent->m_groups++;
}

KGpgGroupNode::KGpgGroupNode(KGpgRootNode *parent, const QString &name, const KGpgKeyNode::List &members)
	: KGpgExpandableNode(parent),
	d_ptr(new KGpgGroupNodePrivate(name))
{
	Q_ASSERT(members.count() > 0);

	foreach (KGpgKeyNode *nd, members)
		new KGpgGroupMemberNode(this, nd);

	parent->m_groups++;
}

KGpgGroupNode::~KGpgGroupNode()
{
	KGpgRootNode *root = m_parent->toRootNode();

	if (root != NULL)
		root->m_groups--;
}

KgpgCore::KgpgItemType
KGpgGroupNode::getType() const
{
	return ITYPE_GROUP;
}

QString
KGpgGroupNode::getName() const
{
	const Q_D(KGpgGroupNode);

	return d->m_name;
}

QString
KGpgGroupNode::getSize() const
{
    return tr("1 key", "%1 keys").arg(children.count());
}

void
KGpgGroupNode::readChildren()
{
}

void
KGpgGroupNode::rename(const QString &newName)
{
	Q_D(KGpgGroupNode);

	QFile conffile;
	QTextStream t;
	QStringList lines;
	int index = d->findGroupEntry(conffile, t, lines);

	// check if file opening failed
	if (!t.device())
		return;

	if (index < 0) {
        qDebug() << "Group " << d->m_name << " not renamed, group does not exist";
		return;
	}

	// 6 = groupTag().length()
	const QString values = lines[index].simplified().mid(6 + d->m_name.length());
	lines[index] = d->groupTag() + newName + QLatin1Char(' ') + values;

	conffile.resize(0);
	t << lines.join(QLatin1String("\n")) + QLatin1Char('\n');

	d->m_name = newName;
}

void
KGpgGroupNode::saveMembers()
{
	Q_D(KGpgGroupNode);

	QFile conffile;
	QTextStream t;
	QStringList lines;
	int index = d->findGroupEntry(conffile, t, lines);

	// check if file opening failed
	if (!t.device())
		return;

	QStringList memberIds;

	for (int j = getChildCount() - 1; j >= 0; j--)
		memberIds << getChild(j)->toGroupMemberNode()->getId();

	const QString groupEntry = d->groupTag() + d->m_name + QLatin1String(" = ") +
			memberIds.join(QLatin1String(" "));

	if (index >= 0)
		lines[index] = groupEntry;
	else
		lines << groupEntry;

	conffile.resize(0);
	t << lines.join(QLatin1String("\n")) + QLatin1Char('\n');
}

void
KGpgGroupNode::remove()
{
	Q_D(KGpgGroupNode);

	QFile conffile;
	QTextStream t;
	QStringList lines;
	int index = d->findGroupEntry(conffile, t, lines);

	// check if file opening failed
	if (!t.device())
		return;

	if (index < 0)
		return;

	lines.removeAt(index);
	conffile.resize(0);
	t << lines.join(QLatin1String("\n")) + QLatin1Char('\n');
}

QStringList
KGpgGroupNode::readGroups()
{
	QStringList groups;
    //QFile qfile(KGpgSettings::gpgConfigPath());
    QFile qfile("");

	if (!qfile.exists() || !qfile.open(QIODevice::ReadOnly))
		return groups;

	QTextStream t(&qfile);

	while (!t.atEnd()) {
		QString line = t.readLine().simplified().section(QLatin1Char('#'), 0, 0);
		if (!KGpgGroupNodePrivate::groupPattern().exactMatch(line))
			continue;

		// remove the "group " at the start
		line.remove(0, 6);
		// transform it in a simple space separated list
		groups.append(line.replace(QLatin1Char('='), QLatin1Char(' ')).simplified());
	}

	return groups;
}
