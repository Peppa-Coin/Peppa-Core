// Copyright (c) 2011-2020 The Peppacoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef PEPPACOIN_QT_PEPPACOINADDRESSVALIDATOR_H
#define PEPPACOIN_QT_PEPPACOINADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class PeppacoinAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit PeppacoinAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

/** Peppacoin address widget validator, checks for a valid bitcoin address.
 */
class PeppacoinAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit PeppacoinAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

#endif // PEPPACOIN_QT_PEPPACOINADDRESSVALIDATOR_H
