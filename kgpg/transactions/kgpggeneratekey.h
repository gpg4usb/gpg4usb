/*
 * Copyright (C) 2008,2009,2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KGPGGENERATEKEY_H
#define KGPGGENERATEKEY_H

#include "kgpgtransaction.h"

#include "../core/kgpgkey.h"

#include <QObject>

class QString;

/**
 * @brief generate a new key pair
 */
class KGpgGenerateKey: public KGpgTransaction {
	Q_OBJECT

	Q_DISABLE_COPY(KGpgGenerateKey)
	KGpgGenerateKey(); // = delete C++0x
public:
	enum ts_generatekey {
		TS_INVALID_NAME = TS_COMMON_END + 1	///< the owners name is not accepted by GnuPG
	};
	/**
	 * @brief KGpgGenerateKey's constructor
	 * @param parent parent object
	 * @param name the name of the key, it is also the user's name.
	 * @param email email MUST be a valid email address or an empty string.
	 * @param comment is a comment, it can be an empty string
	 * @param algorithm this is the type of the key, RSA or DSA & ELGAMAL (\see Kgpg::KeyAlgo ).
	 * @param size this is the length of the key (1024, 2048, ...)
	 * @param expire defines the key expiry time together with \em expireunit, 0 for unlimited key lifetime
	 * @param expireunit is the unit of the number given as \em expire. \see setExpire
	 */
	KGpgGenerateKey(QObject *parent, const QString &name, const QString &email, const QString &comment,
			 const KgpgCore::KgpgKeyAlgo &algorithm, const uint size, const unsigned int expire = 0,
                    const char expireunit = 'd', const QString &password = "");
	virtual ~KGpgGenerateKey();

	void setName(const QString &name);
	QString getName() const;
	void setEmail(const QString &email);
	QString getEmail() const;
	void setComment(const QString &comment);
	void setAlgorithm(const KgpgCore::KgpgKeyAlgo &algorithm);
	void setSize(const unsigned int size);
    void setPassword(const QString &password);
	/**
	 * @brief set expire date for key
	 * @param expire defines the key expiry time together with \em expireunit, 0 for unlimited key lifetime
	 * @param expireunit is the unit of the number given as \em expire.
	 *
	 * Valid units are 'd', 'w', 'm' and 'y'. The unit is ignored if expire is 0.
	 */
	void setExpire(const unsigned int expire, const char expireunit);

	QString getFingerprint() const;

	/**
	 * @brief get error output of GnuPG
	 * @return the messages GnuPG printed to standard error
	 *
	 * This will only return data after the done() signal has been emitted.
	 */
	QString gpgErrorMessage() const;

protected:
	virtual bool preStart();
	virtual void postStart();
	virtual bool nextLine(const QString &line);
	virtual void finish();
	virtual void newPasswordEntered();

private:
	QString m_name;
	QString m_email;
	QString m_comment;
    QString m_password;
	KgpgCore::KgpgKeyAlgo m_algorithm;
	unsigned int m_size;
	unsigned int m_expire;
	unsigned int m_expireunit;
	QString m_fingerprint;
	bool m_namesent;
	QString m_errorOutput;
};

#endif // KGPGGENERATEKEY_H
