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
#include "KGpgRootNode.h"

#include "KGpgGroupNode.h"
#include "kgpginterface.h"
#include "KGpgOrphanNode.h"
//#include "kgpgsettings.h"

#include <QString>
#include <QStringList>

KGpgRootNode::KGpgRootNode(KGpgItemModel *model)
	: KGpgExpandableNode(NULL),
	m_groups(0),
	m_deleting(false)
{
	m_model = model;
}

KGpgRootNode::~KGpgRootNode()
{
	m_deleting = true;
}

void
KGpgRootNode::readChildren()
{
}

KgpgCore::KgpgItemType
KGpgRootNode::getType() const
{
	return 0;
}

void
KGpgRootNode::addGroups(const QStringList &groups)
{
	foreach (const QString &group, groups) {
		QStringList members = group.split(QLatin1Char(' '));
		const QString groupName = members.takeFirst();
		new KGpgGroupNode(this, groupName, members);
	}
}

void
KGpgRootNode::addKeys(const QStringList &ids)
{
	KgpgCore::KgpgKeyList publiclist = KgpgInterface::readPublicKeys(ids);
	KgpgCore::KgpgKeyList secretlist = KgpgInterface::readSecretKeys();

	QStringList issec = secretlist;

	for (int i = 0; i < publiclist.size(); ++i) {
		KgpgCore::KgpgKey key = publiclist.at(i);

		int index = issec.indexOf(key.fullId());
		if (index != -1) {
			key.setSecret(true);
			issec.removeAt(index);
			secretlist.removeAt(index);
		}

		KGpgKeyNode *nd = new KGpgKeyNode(this, key);
		emit newKeyNode(nd);
	}

	for (int i = 0; i < secretlist.count(); ++i) {
		KgpgCore::KgpgKey key = secretlist.at(i);

		new KGpgOrphanNode(this, key);
	}
}

void
KGpgRootNode::refreshKeys(KGpgKeyNode::List nodes)
{
	QStringList ids;

	foreach (const KGpgNode *nd, nodes)
		ids << nd->getId();

	KgpgCore::KgpgKeyList publiclist = KgpgInterface::readPublicKeys(ids);
	QStringList issec = KgpgInterface::readSecretKeys(ids);

	for (int i = 0; i < publiclist.size(); ++i) {
		KgpgCore::KgpgKey key = publiclist.at(i);

		int index = issec.indexOf(key.fullId());
		if (index != -1) {
			key.setSecret(true);
			issec.removeAt(index);
		}

		for (int j = 0; j < nodes.count(); j++) {
			KGpgKeyNode *nd = nodes.at(j);

			if (nd->getId() == key.fingerprint()) {
				nodes.removeAt(j);
				nd->setKey(key);
				break;
			}
		}
	}
}

KGpgKeyNode *
KGpgRootNode::findKey(const QString &keyId)
{
	int i = findKeyRow(keyId);
	if (i >= 0) {
		return children[i]->toKeyNode();
	}

	return NULL;
}

int
KGpgRootNode::findKeyRow(const QString &keyId)
{
	int i = 0;

	foreach (const KGpgNode *node, children) {
		if ((node->getType() & ITYPE_PAIR) == 0) {
			++i;
			continue;
		}

		const KGpgKeyNode *key = node->toKeyNode();

		if (key->compareId(keyId))
			return i;
		++i;
	}
	return -1;
}

int
KGpgRootNode::groupChildren() const
{
	return m_groups;
}

int
KGpgRootNode::findKeyRow(const KGpgKeyNode *key)
{
	for (int i = 0; i < children.count(); i++) {
		if (children[i] == key)
			return i;
	}
	return -1;
}

KGpgRootNode *
KGpgRootNode::asRootNode()
{
	if (m_deleting)
		return NULL;

	return this;
}

const KGpgRootNode *
KGpgRootNode::asRootNode() const
{
	if (m_deleting)
		return NULL;

	return this;
}

//#include "KGpgRootNode.moc"
