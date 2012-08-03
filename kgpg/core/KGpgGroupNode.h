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
#ifndef KGPGGROUPNODE_H
#define KGPGGROUPNODE_H

#include "KGpgExpandableNode.h"
#include "KGpgKeyNode.h"

class QString;
class QStringList;

class KGpgGroupNodePrivate;

/**
 * @brief A GnuPG group of public keys
 */
class KGpgGroupNode : public KGpgExpandableNode
{
private:
	KGpgGroupNodePrivate * const d_ptr;
	Q_DECLARE_PRIVATE(KGpgGroupNode)
	Q_DISABLE_COPY(KGpgGroupNode)

protected:
	virtual void readChildren();

public:
	KGpgGroupNode(KGpgRootNode *parent, const QString &name, const QStringList &members);
	KGpgGroupNode(KGpgRootNode *parent, const QString &name, const KGpgKeyNode::List &members);
	virtual ~KGpgGroupNode();

	virtual KgpgCore::KgpgItemType getType() const;
	/**
	 * Return size of group
	 *
	 * @return the number of keys in this group
	 */
	virtual QString getSize() const;
	virtual QString getName() const;

	/**
	 * Rename this group node
	 *
	 * @param newName new name of the group
	 */
	void rename(const QString &newName);

	/**
	 * Write the current members to GnuPG config file
	 */
	void saveMembers();

	/**
	 * Remove this group from the GnuPG config file
	 */
	void remove();

	/**
	 * @brief get all groups from GnuPG config file
	 * @return list of groups names and their keys
	 *
	 * The strings are themself space separated list. The first entry is the
	 * group name, the others are the keys inside
	 */
	static QStringList readGroups();
};

#endif /* KGPGGROUPNODE_H */
