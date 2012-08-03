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
#ifndef KGPGKEYNODE_H
#define KGPGKEYNODE_H

#include "KGpgSignableNode.h"
#include "KGpgSignNode.h"

#include "kgpgkey.h"

class KGpgExpandableNode;
class KGpgRefNode;

/**
 * @brief A public key with or without corresponding secret key
 */
class KGpgKeyNode : public KGpgSignableNode
{
	Q_OBJECT

	friend class KGpgGroupMemberNode;

private:
	KgpgCore::KgpgKey *m_key;
	int m_signs;

protected:
	virtual void readChildren();

	QList<KGpgRefNode *> m_refs;
	QList<KGpgRefNode *> getRefsOfType(const KgpgCore::KgpgItemType &type) const;

public:
	typedef QList<KGpgKeyNode *> List;
	typedef QList<const KGpgKeyNode *> ConstList;

	explicit KGpgKeyNode(KGpgRootNode *parent, const KgpgCore::KgpgKey &k);
	virtual ~KGpgKeyNode();

	virtual bool hasChildren() const;

	static KgpgCore::KgpgItemType getType(const KgpgCore::KgpgKey *k);

	virtual KgpgCore::KgpgItemType getType() const;
	virtual KgpgCore::KgpgKeyTrust getTrust() const;
	const QString &getFingerprint() const;
	virtual QString getSize() const;
	virtual QString getName() const;
	virtual QString getEmail() const;
	virtual QDateTime getExpiration() const;
	virtual QDateTime getCreation() const;
	virtual QString getId() const;
	virtual KGpgKeyNode *getKeyNode(void);
	virtual const KGpgKeyNode *getKeyNode(void) const;
	/**
	 * @brief Return if this key has a private key
	 */
	bool isSecret() const;
	/**
	 * @brief Print the full key fingerprint with spaces inserted
	 *
	 * For display purposes you normally don't want to print the full
	 * fingerprint as is because it's too many hex characters at once.
	 * This function returns the fingerprint in the format usually used
	 * for printing this out, i.e. with a space after each fourth hex
	 * character.
	 *
	 * @return the full fingerprint with spaces inserted
	 */
	QString getBeautifiedFingerprint() const;
	virtual QString getComment() const;
	/**
	 * @brief Return the number of signatures of the primary user id
	 *
	 * This is different from the number of children of this node as there
	 * is usually at least one subkey and there may also be additional
	 * user ids or attributes. This does not count the signatures to those
	 * slave objects, only the ones that are direct children of this node.
	 *
	 * @return the number of signatures to the primary user id
	 */
	virtual QString getSignCount() const;
	/**
	 * @brief Creates a copy of the KgpgKey that belongs to this class
	 */
	virtual KgpgCore::KgpgKey *copyKey() const;
	/**
	 * @brief Replaces the current key information with the new one.
	 * All sub-items (i.e. signatures, user ids ...) will be deleted. This must
	 * only be used when the id of both new and old key is the same.
	 */
	void setKey(const KgpgCore::KgpgKey &key);
	/**
	 * @brief Returns a reference to the key used in this object.
	 * This allows direct access to the values of the key e.g. for KgpgKeyInfo.
	 */
	const KgpgCore::KgpgKey *getKey() const;

	/**
	 * @brief Returns the size of the signing key.
	 * @return signing key size in bits
	 */
	virtual unsigned int getSignKeySize() const;
	/**
	 * @brief Returns the size of the first encryption subkey.
	 * @return encryption key size in bits
	 */
	virtual unsigned int getEncryptionKeySize() const;
	/**
	 * @brief Notify this key that a KGpgRefNode now references this key.
	 * @param node object that takes the reference
	 */
	void addRef(KGpgRefNode *node);
	/**
	 * @brief Remove a reference to this object
	 * @param node node that no longer has the reference
	 *
	 * Note that this must not be called as reply when this object
	 * emits updated(NULL)
	 */
	void delRef(KGpgRefNode *node);
	/**
	 * @brief returns a list of all groups this key is member of
	 */
	QList<KGpgGroupNode *> getGroups(void) const;
	/**
	 * @brief returns a list of all group member nodes that reference this key
	 */
	QList<KGpgGroupMemberNode *> getGroupRefs(void) const;
	/**
	 * @brief returns a list of all sign nodes that reference this key
	 */
	KGpgSignNode::List getSignRefs(void) const;
	/**
	 * @brief returns a list of signatures to this key
	 * @param subkeys if signatures on subkeys should be included
	 */
	KGpgSignNode::List getSignatures(const bool subkeys) const;
	/**
	 * @brief get the user id or user attribute with the given number
	 * @param index the index of the user id to return
	 * @return the requested subitem or NULL if that is not present
	 *
	 * User ids indexes are 1-based, so 0 is not a valid index. Passing
	 * 1 as index will return the object itself, representing the primary
	 * user id.
	 */
	const KGpgSignableNode *getUid(const unsigned int index) const;

	/**
	 * @brief compare the id of this node to the given other node
	 * @param other key id to compare to
	 * @return if ids are identical
	 *
	 * This handles different length of the id string.
	 */
	bool compareId(const QString &other) const;
Q_SIGNALS:
	void expanded();

public slots:
	/**
	 * @brief read all subitems
	 *
	 * This will read in all subitems (e.g. subkeys, signatures). When
	 * this is done the expanded() signal is emitted. The signal is emitted
	 * immediately if the key has been expanded before.
	 *
	 * This will not update the child items in case they are already present.
	 * Use KGpgItemModel::refreshKey() instead.
	 */
	void expand();
};

#endif /* KGPGKEYNODE_H */
