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
#ifndef KGPGORPHANNODE_H
#define KGPGORPHANNODE_H

#include "KGpgNode.h"

#include "kgpgkey.h"

using namespace KgpgCore;

class KGpgExpandableNode;

/**
 * @brief A lone secret key without public key
 */
class KGpgOrphanNode : public KGpgNode
{
private:
	KgpgCore::KgpgKey *m_key;

public:
	explicit KGpgOrphanNode(KGpgExpandableNode *parent, const KgpgKey &k);
	virtual ~KGpgOrphanNode();

	virtual KgpgCore::KgpgItemType getType() const;
	virtual KgpgCore::KgpgKeyTrust getTrust() const;
	const QString &getFingerprint() const;
	virtual QString getSize() const;
	virtual QString getName() const;
	virtual QString getEmail() const;
	virtual QDateTime getExpiration() const;
	virtual QDateTime getCreation() const;
	virtual QString getId() const;
};

#endif /* KGPGORPHANNODE_H */
