/* Copyright 2008,2009 Rolf Eike Beer <kde@opensource.sf-tec.de>
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
#ifndef KGPGGROUPMEMBERNODE_H
#define KGPGGROUPMEMBERNODE_H

#include "KGpgRefNode.h"

#include <QPixmap>
#include "kgpgkey.h"

using namespace KgpgCore;

class KGpgKeyNode;
class KGpgGroupNode;

/**
 * @brief A member of a GnuPG group
 */
class KGpgGroupMemberNode : public KGpgRefNode
{
public:
	explicit KGpgGroupMemberNode(KGpgGroupNode *parent, const QString &k);
	explicit KGpgGroupMemberNode(KGpgGroupNode *parent, KGpgKeyNode *k);
	virtual ~KGpgGroupMemberNode();

	virtual KgpgCore::KgpgKeyTrust getTrust() const;
	virtual KgpgCore::KgpgItemType getType() const;
	virtual QString getSize() const;
	virtual QDateTime getExpiration() const;
	virtual QDateTime getCreation() const;
	virtual KGpgGroupNode *getParentKeyNode() const;

	/**
	 * Returns the size of the signing key.
	 * @return signing key size in bits
	 */
	virtual unsigned int getSignKeySize() const;
	/**
	 * Returns the size of the first encryption subkey.
	 * @return encryption key size in bits
	 */
	virtual unsigned int getEncryptionKeySize() const;
};

#endif /* KGPGGROUPMEMBERNODE_H */
