/*
 * Copyright (C) 2006,2007 Jimmy Gilles <jimmygilles@gmail.com>
 * Copyright (C) 2007,2008,2009,2010,2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kgpgkey.h"

#include "convert.h"

//#include <KLocale>

#include <QStringList>

namespace KgpgCore
{

//BEGIN KeySub
KgpgKeySubPrivate::KgpgKeySubPrivate(const QString &id, const uint size, const KgpgKeyTrust trust, const KgpgKeyAlgo algo,
                                     const KgpgSubKeyType type, const QDateTime &date)
    : gpgsubid(id),
    gpgsubsize(size),
    gpgsubcreation(date),
    gpgsubtrust(trust),
    gpgsubalgo(algo),
    gpgsubtype(type)
{
}

bool KgpgKeySubPrivate::operator==(const KgpgKeySubPrivate &other) const
{
    if (gpgsubvalid != other.gpgsubvalid) return false;
    if (gpgsubalgo != other.gpgsubalgo) return false;
    if (gpgsubid != other.gpgsubid) return false;
    if (gpgsubsize != other.gpgsubsize) return false;
    if (gpgsubexpiration != other.gpgsubexpiration) return false;
    if (gpgsubcreation != other.gpgsubcreation) return false;
    if (gpgsubtrust != other.gpgsubtrust) return false;
    if (gpgsubtype != other.gpgsubtype) return false;
    return true;
}

KgpgKeySub::KgpgKeySub(const QString &id, const uint size, const KgpgKeyTrust trust, const KgpgKeyAlgo algo, const KgpgSubKeyType type,
                       const QDateTime &date)
    : d(new  KgpgKeySubPrivate(id, size, trust, algo, type, date))
{
    d->gpgsubvalid = false;
}

KgpgKeySub::KgpgKeySub(const KgpgKeySub &other)
{
    d = other.d;
}

void KgpgKeySub::setExpiration(const QDateTime &date)
{
    d->gpgsubexpiration = date;
}

void KgpgKeySub::setValid(const bool valid)
{
    d->gpgsubvalid = valid;
}

QString KgpgKeySub::id() const
{
    return d->gpgsubid;
}

uint KgpgKeySub::size() const
{
    return d->gpgsubsize;
}

bool KgpgKeySub::unlimited() const
{
    return d->gpgsubexpiration.isNull();
}

QDateTime KgpgKeySub::expirationDate() const
{
    return d->gpgsubexpiration;
}

QDateTime KgpgKeySub::creationDate() const
{
    return d->gpgsubcreation;
}

KgpgKeyTrust KgpgKeySub::trust() const
{
    return d->gpgsubtrust;
}

KgpgKeyAlgo KgpgKeySub::algorithm() const
{
    return d->gpgsubalgo;
}

bool KgpgKeySub::valid() const
{
    return d->gpgsubvalid;
}

KgpgSubKeyType KgpgKeySub::type() const
{
    return d->gpgsubtype;
}

bool KgpgKeySub::operator==(const KgpgKeySub &other) const
{
    if (d == other.d) return true;
    if ((*d) == (*(other.d))) return true;
    return false;
}

KgpgKeySub& KgpgKeySub::operator=(const KgpgKeySub &other)
{
    d = other.d;
    return *this;
}

//END KeySub


//BEGIN Key

KgpgKeyPrivate::KgpgKeyPrivate(const QString &id, const uint size, const KgpgKeyTrust trust, const KgpgKeyAlgo algo, const QDateTime &date)
    : gpgkeysecret(false),
    gpgkeyvalid(false),
    gpgkeyid(id),
    gpgkeysize(size),
    gpgkeytrust(trust),
    gpgkeycreation(date),
    gpgkeyalgo(algo),
    gpgsublist(new KgpgKeySubList())
{
}

bool KgpgKeyPrivate::operator==(const KgpgKeyPrivate &other) const
{
    if (gpgkeysecret != other.gpgkeysecret) return false;
    if (gpgkeyvalid != other.gpgkeyvalid) return false;
    if (gpgkeymail != other.gpgkeymail) return false;
    if (gpgkeyname != other.gpgkeyname) return false;
    if (gpgkeycomment != other.gpgkeycomment) return false;
    if (gpgkeyfingerprint != other.gpgkeyfingerprint) return false;
    if (gpgkeyid != other.gpgkeyid) return false;
    if (gpgkeysize != other.gpgkeysize) return false;
    if (gpgkeyownertrust != other.gpgkeyownertrust) return false;
    if (gpgkeytrust != other.gpgkeytrust) return false;
    if (gpgkeycreation != other.gpgkeycreation) return false;
    if (gpgkeyexpiration != other.gpgkeyexpiration) return false;
    if (gpgkeyalgo != other.gpgkeyalgo) return false;
    if (gpgsublist != other.gpgsublist) return false;
    return true;
}

KgpgKey::KgpgKey(const QString &id, const uint size, const KgpgKeyTrust trust, const KgpgKeyAlgo algo, const QDateTime &date)
    : d(new KgpgKeyPrivate(id, size, trust, algo, date))
{
}

KgpgKey::KgpgKey(const KgpgKey &other)
{
    d = other.d;
}

void KgpgKey::setSecret(const bool secret)
{
    d->gpgkeysecret = secret;
}

void KgpgKey::setValid(const bool valid)
{
    d->gpgkeyvalid = valid;
}

void KgpgKey::setName(const QString &name)
{
    d->gpgkeyname = name;
}

void KgpgKey::setEmail(const QString &email)
{
    d->gpgkeymail = email;
}

void KgpgKey::setComment(const QString &comment)
{
    d->gpgkeycomment = comment;
}

void KgpgKey::setFingerprint(const QString &fingerprint)
{
    d->gpgkeyfingerprint = fingerprint;
}

void KgpgKey::setOwnerTrust(const KgpgKeyOwnerTrust &owtrust)
{
    d->gpgkeyownertrust = owtrust;
}

void KgpgKey::setExpiration(const QDateTime &date)
{
    d->gpgkeyexpiration = date;
}

bool KgpgKey::secret() const
{
    return d->gpgkeysecret;
}

bool KgpgKey::valid() const
{
    return d->gpgkeyvalid;
}

QString KgpgKey::id() const
{
    return d->gpgkeyid.right(8);
}

QString KgpgKey::fullId() const
{
    return d->gpgkeyid;
}

QString KgpgKey::name() const
{
    return d->gpgkeyname;
}

QString KgpgKey::email() const
{
    return d->gpgkeymail;
}

QString KgpgKey::comment() const
{
    return d->gpgkeycomment;
}

const QString &KgpgKey::fingerprint() const
{
    return d->gpgkeyfingerprint;
}

QString KgpgKey::fingerprintBeautified() const
{
    QString fingervalue =d->gpgkeyfingerprint;
    uint len = fingervalue.length();
    if ((len > 0) && (len % 4 == 0))
      for (uint n = 0; 4 * (n + 1) < len; ++n)
        fingervalue.insert(5 * n + 4, QLatin1Char( ' ' ));
    return fingervalue;
}

uint KgpgKey::size() const
{
    return d->gpgkeysize;
}

uint KgpgKey::encryptionSize() const
{
	// Get the first encryption subkey
	for (int i = 0; i < d->gpgsublist->count(); ++i) {
		KgpgKeySub temp = d->gpgsublist->at(i);
		if (temp.type() & SKT_ENCRYPTION) {
			return temp.size();
		}
	}
	return 0;
}

KgpgKeyOwnerTrust KgpgKey::ownerTrust() const
{
    return d->gpgkeyownertrust;
}

KgpgKeyTrust KgpgKey::trust() const
{
    return d->gpgkeytrust;
}

QDateTime KgpgKey::creationDate() const
{
    return d->gpgkeycreation;
}

QDateTime KgpgKey::expirationDate() const
{
    return d->gpgkeyexpiration;
}

bool KgpgKey::unlimited() const
{
    return d->gpgkeyexpiration.isNull();
}

KgpgKeyAlgo KgpgKey::algorithm() const
{
    return d->gpgkeyalgo;
}

KgpgKeyAlgo KgpgKey::encryptionAlgorithm() const
{
	// Get the first encryption subkey
	for (int i = 0; i < d->gpgsublist->count(); ++i) {
		KgpgKeySub temp = d->gpgsublist->at(i);
		if (temp.type() & SKT_ENCRYPTION) {
			return temp.algorithm();
		}
	}
	return ALGO_UNKNOWN;
}

KgpgKeySubListPtr KgpgKey::subList() const
{
    return d->gpgsublist;
}

bool KgpgKey::operator==(const KgpgKey &other) const
{
    if (d == other.d) return true;
    if ((*d) == (*(other.d))) return true;
    return false;
}

KgpgKey& KgpgKey::operator=(const KgpgKey &other)
{
    d = other.d;
    return *this;
}

KgpgKeyList::operator QStringList() const
{
    QStringList res;
    foreach(const KgpgKey &key, *this)
        res << key.fullId();
    return res;
}

//END Key

} // namespace KgpgCore
