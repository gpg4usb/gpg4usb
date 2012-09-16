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
#include "KGpgKeyNode.h"

#include "KGpgGroupMemberNode.h"
#include "KGpgRootNode.h"
#include "KGpgSubkeyNode.h"
#include "KGpgUatNode.h"
#include "KGpgUidNode.h"
#include "convert.h"
#include "../kgpginterface.h"
//#include "kgpgsettings.h"
//#include "model/kgpgitemmodel.h"

//#include <KLocale>

KGpgKeyNode::KGpgKeyNode(KGpgRootNode *parent, const KgpgCore::KgpgKey &k)
	: KGpgSignableNode(parent),
	m_key(new KgpgCore::KgpgKey(k)),
	m_signs(0)
{
}

KGpgKeyNode::~KGpgKeyNode()
{
	foreach (KGpgRefNode *nd, m_refs) {
		nd->unRef(m_parent->toRootNode());
	}
}

KgpgCore::KgpgItemType
KGpgKeyNode::getType() const
{
	return getType(m_key);
}

bool
KGpgKeyNode::hasChildren() const
{
	return true;
}

KgpgCore::KgpgItemType
KGpgKeyNode::getType(const KgpgCore::KgpgKey *k)
{
	if (k->secret())
		return KgpgCore::ITYPE_PAIR;

	return KgpgCore::ITYPE_PUBLIC;
}

KgpgCore::KgpgKeyTrust
KGpgKeyNode::getTrust() const
{
	return m_key->trust();
}

const QString &
KGpgKeyNode::getFingerprint() const
{
	return m_key->fingerprint();
}

QString
KGpgKeyNode::getSize() const
{
    return "size of signing key / size of encryption key",
			"%1 / %2", QString::number(getSignKeySize()),
            QString::number(getEncryptionKeySize());
}

QString
KGpgKeyNode::getName() const
{
	return m_key->name();
}

QString
KGpgKeyNode::getEmail() const
{
	return m_key->email();
}

QDateTime
KGpgKeyNode::getExpiration() const
{
	return m_key->expirationDate();
}

QDateTime
KGpgKeyNode::getCreation() const
{
	return m_key->creationDate();
}

QString
KGpgKeyNode::getId() const
{
	return m_key->fullId();
}

KGpgKeyNode *
KGpgKeyNode::getKeyNode(void)
{
	return this;
}

bool
KGpgKeyNode::isSecret() const
{
	return m_key->secret();
}

const KGpgKeyNode *
KGpgKeyNode::getKeyNode(void) const
{
	return this;
}

QString
KGpgKeyNode::getBeautifiedFingerprint() const
{
	return m_key->fingerprintBeautified();
}

QString
KGpgKeyNode::getComment() const
{
	return m_key->comment();
}

void
KGpgKeyNode::readChildren()
{
	KgpgInterface::readSignatures(this);

	m_signs = 0;
	foreach(KGpgNode *n, children)
		if (n->getType() == ITYPE_SIGN)
			m_signs++;
}

QString
KGpgKeyNode::getSignCount() const
{
	if (!wasExpanded())
		return QString();
    //return i18np("1 signature", "%1 signatures", m_signs);
    return "TODO";
}

KgpgKey *
KGpgKeyNode::copyKey() const
{
	return new KgpgKey(*m_key);
}

void
KGpgKeyNode::setKey(const KgpgKey &key)
{
	Q_ASSERT(m_key->fingerprint() == key.fingerprint());
	delete m_key;

	for (int i = 0; i < children.count(); i++)
		delete children.at(i);
	children.clear();

	m_key = new KgpgKey(key);
}

const KgpgKey *
KGpgKeyNode::getKey() const
{
	return m_key;
}

unsigned int
KGpgKeyNode::getSignKeySize() const
{
	return m_key->size();
}

unsigned int
KGpgKeyNode::getEncryptionKeySize() const
{
	return m_key->encryptionSize();
}

void
KGpgKeyNode::addRef(KGpgRefNode *node)
{
	Q_ASSERT(m_refs.indexOf(node) == -1);
	m_refs.append(node);
}

void
KGpgKeyNode::delRef(KGpgRefNode *node)
{
	Q_ASSERT(m_refs.indexOf(node) != -1);
	m_refs.removeOne(node);
	Q_ASSERT(m_refs.indexOf(node) == -1);
}

QList<KGpgGroupNode *>
KGpgKeyNode::getGroups(void) const
{
	QList<KGpgGroupNode *> ret;

	foreach (KGpgGroupMemberNode *gnd, getGroupRefs())
		ret.append(gnd->getParentKeyNode());

	return ret;
}

QList<KGpgRefNode *>
KGpgKeyNode::getRefsOfType(const KgpgItemType &type) const
{
	QList<KGpgRefNode *> ret;

	foreach (KGpgRefNode *nd, m_refs) {
		if (nd->getType() & type)
			ret.append(nd);
	}

	return ret;
}

QList<KGpgGroupMemberNode *>
KGpgKeyNode::getGroupRefs(void) const
{
	QList<KGpgGroupMemberNode *> ret;

	foreach (KGpgRefNode *rn, getRefsOfType(KgpgCore::ITYPE_GROUP))
		ret.append(rn->toGroupMemberNode());

	return ret;
}

KGpgSignNode::List
KGpgKeyNode::getSignRefs(void) const
{
	KGpgSignNode::List ret;

	QList<KGpgRefNode *> refs = getRefsOfType(KgpgCore::ITYPE_SIGN);

	foreach (KGpgRefNode *rn, refs)
		ret.append(rn->toSignNode());

	return ret;
}

KGpgSignNode::List
KGpgKeyNode::getSignatures(const bool subkeys) const
{
	KGpgSignNode::List ret = KGpgSignableNode::getSignatures();

	if (!subkeys)
		return ret;

	foreach (KGpgNode *child, children) {
		KGpgSignNode::List tmp;

		switch (child->getType()) {
		case KgpgCore::ITYPE_UID:
		case KgpgCore::ITYPE_UAT:
			tmp = child->toSignableNode()->getSignatures();
			break;
		default:
			continue;
		}

		foreach (KGpgSignNode *sn, tmp) {
			bool found = false;
			const QString snid(sn->getId());

			foreach (const KGpgSignNode *retsn, ret) {
				found = (retsn->getId() == snid);
				if (found)
					break;
			}

			if (!found)
				ret << sn;
		}
	}

	return ret;
}

const KGpgSignableNode *
KGpgKeyNode::getUid(const unsigned int index) const
{
	Q_ASSERT(index > 0);

	if (index == 1)
		return this;

	const QString idxstr(QString::number(index));

	foreach (const KGpgNode *child, children) {
		KGpgSignNode::List tmp;

		switch (child->getType()) {
		case KgpgCore::ITYPE_UID:
		case KgpgCore::ITYPE_UAT:
			if (child->getId() == idxstr)
				return child->toSignableNode();
			break;
		default:
			continue;
		}
	}

	return NULL;
}

bool
KGpgKeyNode::compareId(const QString &other) const
{
	if (other.length() == m_key->fullId().length())
		return (other.compare(m_key->fullId(), Qt::CaseInsensitive) == 0);

	if (other.length() == m_key->fingerprint().length())
		return (other.compare(m_key->fingerprint(), Qt::CaseInsensitive) == 0);

	const QString comId = m_key->fullId().isEmpty() ? m_key->fingerprint() : m_key->fullId();

	return (other.right(comId.length()).compare(
			comId.right(other.length()),
			Qt::CaseInsensitive) == 0);
}

void
KGpgKeyNode::expand()
{
	if (!wasExpanded())
		readChildren();

	emit expanded();
}

//#include "KGpgKeyNode.moc"
