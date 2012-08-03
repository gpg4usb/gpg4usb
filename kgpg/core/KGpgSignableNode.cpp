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
#include "KGpgSignableNode.h"

//#include <KLocale>

KGpgSignableNode::KGpgSignableNode(KGpgExpandableNode *parent)
	: KGpgExpandableNode(parent)
{
}

KGpgSignableNode::~KGpgSignableNode()
{
}

KGpgSignNode::List
KGpgSignableNode::getSignatures(void) const
{
	KGpgSignNode::List ret;

	foreach (KGpgNode *kn, children) {
		if (kn->getType() == KgpgCore::ITYPE_SIGN)
			ret << kn->toSignNode();
	}

	return ret;
}

QString
KGpgSignableNode::getSignCount() const
{
    //return i18np("1 signature", "%1 signatures", children.count());
    return tr("1 signature", "%1 signatures", children.count());
}

bool
KGpgSignableNode::operator<(const KGpgSignableNode &other) const
{
	return operator<(&other);
}

bool
KGpgSignableNode::operator<(const KGpgSignableNode *other) const
{
	switch (getType()) {
	case KgpgCore::ITYPE_PUBLIC:
	case KgpgCore::ITYPE_PAIR: {
		const QString myid(getId());

		switch (other->getType()) {
		case KgpgCore::ITYPE_PUBLIC:
		case KgpgCore::ITYPE_PAIR:
			return (myid < other->getId());
		default: {
			const QString otherid(other->getParentKeyNode()->getId());

			if (myid == otherid)
				return true;
			return (myid < otherid);
		}
		}
	}
	default: {
		const QString myp(getParentKeyNode()->getId());

		switch (other->getType()) {
		case KgpgCore::ITYPE_PAIR:
		case KgpgCore::ITYPE_PUBLIC:
			return (myp < other->getId());
		default: {
			const QString otherp(other->getParentKeyNode()->getId());

			if (otherp == myp)
				return (getId().toInt() < other->getId().toInt());

			return (myp < otherp);
		}
		}
	}
	}
}

//#include "KGpgSignableNode.moc"
