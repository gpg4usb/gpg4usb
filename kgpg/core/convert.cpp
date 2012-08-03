/*
 * Copyright (C) 2006 Jimmy Gilles <jimmygilles@gmail.com>
 * Copyright (C) 2010 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#include "convert.h"

//#include <KGlobal>
//#include <KLocale>
#include <QTranslator>
//#include "kgpgsettings.h"
#include "images.h"

namespace KgpgCore
{

QString Convert::toString(const KgpgKeyAlgo algorithm)
{
    switch (algorithm)
    {
        case ALGO_RSA:          return QObject::tr("Encryption algorithm", "RSA");
        case ALGO_DSA:          return QObject::tr("Encryption algorithm", "DSA");
        case ALGO_ELGAMAL:      return QObject::tr("Encryption algorithm", "ElGamal");
        case ALGO_DSA_ELGAMAL:  return QObject::tr("Encryption algorithm", "DSA & ElGamal");
        case ALGO_RSA_RSA:      return QObject::tr("Encryption algorithm RSA, Signing algorithm RSA", "RSA & RSA");
        case ALGO_UNKNOWN:
        default:                return QObject::tr("Unknown algorithm", "Unknown");
    }
}

QString Convert::toString(const KgpgKeyOwnerTrust ownertrust)
{
    switch (ownertrust)
    {
        case OWTRUST_UNDEFINED: return QObject::tr("Do not Know");
        case OWTRUST_NONE:      return QObject::tr("Do NOT Trust");
        case OWTRUST_MARGINAL:  return QObject::tr("Marginally");
        case OWTRUST_FULL:      return QObject::tr("Fully");
        case OWTRUST_ULTIMATE:  return QObject::tr("Ultimately");
        case OWTRUST_UNKNOWN:
        default:                return QObject::tr("Unknown trust in key owner", "Unknown");
    }
}

QString Convert::toString(const KgpgKeyTrust trust)
{
    switch (trust)
    {
        case TRUST_INVALID:     return QObject::tr("Invalid key", "Invalid");
        case TRUST_DISABLED:    return QObject::tr("Disabled key", "Disabled");
        case TRUST_REVOKED:     return QObject::tr("Revoked");
        case TRUST_EXPIRED:     return QObject::tr("Expired key", "Expired");
        case TRUST_UNDEFINED:   return QObject::tr("Undefined key trust", "Undefined");
        case TRUST_NONE:        return QObject::tr("No trust in key", "None");
        case TRUST_MARGINAL:    return QObject::tr("Marginal trust in key", "Marginal");
        case TRUST_FULL:        return QObject::tr("Full trust in key", "Full");
        case TRUST_ULTIMATE:    return QObject::tr("Ultimate trust in key", "Ultimate");
        case TRUST_UNKNOWN:
        default:                return QObject::tr("Unknown trust in key", "Unknown");
    }
}

QColor Convert::toColor(const KgpgKeyTrust trust)
{
    switch (trust)
    {
        case TRUST_INVALID:
        /*case TRUST_DISABLED:    return KGpgSettings::colorBad();
        case TRUST_EXPIRED:     return KGpgSettings::colorExpired();
        case TRUST_MARGINAL:    return KGpgSettings::colorMarginal();
        case TRUST_REVOKED:     return KGpgSettings::colorRev();
        case TRUST_UNDEFINED:
        case TRUST_NONE:        return KGpgSettings::colorUnknown();
        case TRUST_FULL:        return KGpgSettings::colorGood();
        case TRUST_ULTIMATE:    return KGpgSettings::colorUltimate();*/
        case TRUST_UNKNOWN:
        default:                return QColor(0,0,0);
                            //return KGpgSettings::colorUnknown();
    }
}

QString Convert::toString(const QDate &date)
{
    //return KGlobal::locale()->formatDate(date, KLocale::ShortDate);
}

KgpgKeyAlgo Convert::toAlgo(const uint v)
{
    switch (v)
    {
        case 1:      return ALGO_RSA;
        case 16:
        case 20:     return ALGO_ELGAMAL;
        case 17:     return ALGO_DSA;
        default:     return ALGO_UNKNOWN;
    }
}

KgpgKeyAlgo Convert::toAlgo(const QString &s)
{
    bool b;
    unsigned int u = s.toUInt(&b);
    return b ? toAlgo(u) : ALGO_UNKNOWN;
}

KgpgKeyTrust Convert::toTrust(const QChar &c)
{
    switch (c.toAscii())
    {
        case 'o':    return TRUST_UNKNOWN;
        case 'i':    return TRUST_INVALID;
        case 'd':    return TRUST_DISABLED;
        case 'r':    return TRUST_REVOKED;
        case 'e':    return TRUST_EXPIRED;
        case 'q':    return TRUST_UNDEFINED;
        case 'n':    return TRUST_NONE;
        case 'm':    return TRUST_MARGINAL;
        case 'f':    return TRUST_FULL;
        case 'u':    return TRUST_ULTIMATE;
        default:     return TRUST_UNKNOWN;
    }
}

KgpgKeyTrust Convert::toTrust(const QString &s)
{
    return s.isEmpty() ? TRUST_UNKNOWN : toTrust(s[0]);
}

KgpgKeyOwnerTrust Convert::toOwnerTrust(const QChar &c)
{
    switch (c.toAscii())
    {
        case 'n':     return OWTRUST_NONE;
        case 'm':     return OWTRUST_MARGINAL;
        case 'u':     return OWTRUST_ULTIMATE;
        case 'f':     return OWTRUST_FULL;
        default:      return OWTRUST_UNDEFINED;
    }
}

KgpgKeyOwnerTrust Convert::toOwnerTrust(const QString &s)
{
    return s.isEmpty() ? OWTRUST_UNDEFINED : toOwnerTrust(s[0]);
}

QPixmap Convert::toPixmap(const KgpgItemType t)
{
    switch (t)
    {
        case ITYPE_GROUP:	return Images::group();
        case ITYPE_GSECRET:
        case ITYPE_SECRET:	return Images::orphan();
        case ITYPE_GPUBLIC:
        case ITYPE_SUB:
        case ITYPE_PUBLIC:	return Images::single();
        case ITYPE_GPAIR:
        case ITYPE_PAIR:	return Images::pair();
        case ITYPE_UID:		return Images::userId();
        case ITYPE_UAT:		return Images::photo();
        case ITYPE_REVSIGN:	return Images::revoke();
        case ITYPE_SIGN:	return Images::signature();
	default:		Q_ASSERT(1);
				return NULL;
    }
}

} // namespace KgpgCore
