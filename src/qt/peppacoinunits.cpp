// Copyright (c) 2011-2021 The Peppacoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/peppacoinunits.h>

#include <consensus/amount.h>

#include <QStringList>

#include <cassert>

static constexpr auto MAX_DIGITS_DRI = 16;

PeppacoinUnits::PeppacoinUnits(QObject *parent):
        QAbstractListModel(parent),
        unitlist(availableUnits())
{
}

QList<PeppacoinUnit> PeppacoinUnits::availableUnits()
{
    QList<PeppacoinUnit> unitlist;
    unitlist.append(Unit::DRI);
    unitlist.append(Unit::mDRI);
    unitlist.append(Unit::uDRI);
    unitlist.append(Unit::CAC);
    return unitlist;
}

QString PeppacoinUnits::longName(Unit unit)
{
    switch (unit) {
    case Unit::DRI: return QString("DRI");
    case Unit::mDRI: return QString("mDRI");
    case Unit::uDRI: return QString::fromUtf8("µDRI (bits)");
    case Unit::CAC: return QString("Cactus (cac)");
    } // no default case, so the compiler can warn about missing cases
    assert(false);
}

QString PeppacoinUnits::shortName(Unit unit)
{
    switch (unit) {
    case Unit::DRI: return longName(unit);
    case Unit::mDRI: return longName(unit);
    case Unit::uDRI: return QString("bits");
    case Unit::CAC: return QString("cac");
    } // no default case, so the compiler can warn about missing cases
    assert(false);
}

QString PeppacoinUnits::description(Unit unit)
{
    switch (unit) {
    case Unit::DRI: return QString("Peppa-coins");
    case Unit::mDRI: return QString("Milli-Peppa-coins (1 / 1" THIN_SP_UTF8 "000)");
    case Unit::uDRI: return QString("Micro-Peppa-coins (peppas) (1 / 1" THIN_SP_UTF8 "000" THIN_SP_UTF8 "000)");
    case Unit::CAC: return QString("Cactus (cac) (1 / 100" THIN_SP_UTF8 "000" THIN_SP_UTF8 "000)");
    } // no default case, so the compiler can warn about missing cases
    assert(false);
}

qint64 PeppacoinUnits::factor(Unit unit)
{
    switch (unit) {
    case Unit::DRI: return 100'000'000;
    case Unit::mDRI: return 100'000;
    case Unit::uDRI: return 100;
    case Unit::CAC: return 1;
    } // no default case, so the compiler can warn about missing cases
    assert(false);
}

int PeppacoinUnits::decimals(Unit unit)
{
    switch (unit) {
    case Unit::DRI: return 8;
    case Unit::mDRI: return 5;
    case Unit::uDRI: return 2;
    case Unit::CAC: return 0;
    } // no default case, so the compiler can warn about missing cases
    assert(false);
}

QString PeppacoinUnits::format(Unit unit, const CAmount& nIn, bool fPlus, SeparatorStyle separators, bool justify)
{
    // Note: not using straight sprintf here because we do NOT want
    // localized number formatting.
    qint64 n = (qint64)nIn;
    qint64 coin = factor(unit);
    int num_decimals = decimals(unit);
    qint64 n_abs = (n > 0 ? n : -n);
    qint64 quotient = n_abs / coin;
    QString quotient_str = QString::number(quotient);
    if (justify) {
        quotient_str = quotient_str.rightJustified(MAX_DIGITS_DRI - num_decimals, ' ');
    }

    // Use SI-style thin space separators as these are locale independent and can't be
    // confused with the decimal marker.
    QChar thin_sp(THIN_SP_CP);
    int q_size = quotient_str.size();
    if (separators == SeparatorStyle::ALWAYS || (separators == SeparatorStyle::STANDARD && q_size > 4))
        for (int i = 3; i < q_size; i += 3)
            quotient_str.insert(q_size - i, thin_sp);

    if (n < 0)
        quotient_str.insert(0, '-');
    else if (fPlus && n > 0)
        quotient_str.insert(0, '+');

    if (num_decimals > 0) {
        qint64 remainder = n_abs % coin;
        QString remainder_str = QString::number(remainder).rightJustified(num_decimals, '0');
        return quotient_str + QString(".") + remainder_str;
    } else {
        return quotient_str;
    }
}


// NOTE: Using formatWithUnit in an HTML context risks wrapping
// quantities at the thousands separator. More subtly, it also results
// in a standard space rather than a thin space, due to a bug in Qt's
// XML whitespace canonicalisation
//
// Please take care to use formatHtmlWithUnit instead, when
// appropriate.

QString PeppacoinUnits::formatWithUnit(Unit unit, const CAmount& amount, bool plussign, SeparatorStyle separators)
{
    return format(unit, amount, plussign, separators) + QString(" ") + shortName(unit);
}

QString PeppacoinUnits::formatHtmlWithUnit(Unit unit, const CAmount& amount, bool plussign, SeparatorStyle separators)
{
    QString str(formatWithUnit(unit, amount, plussign, separators));
    str.replace(QChar(THIN_SP_CP), QString(THIN_SP_HTML));
    return QString("<span style='white-space: nowrap;'>%1</span>").arg(str);
}

QString PeppacoinUnits::formatWithPrivacy(Unit unit, const CAmount& amount, SeparatorStyle separators, bool privacy)
{
    assert(amount >= 0);
    QString value;
    if (privacy) {
        value = format(unit, 0, false, separators, true).replace('0', '#');
    } else {
        value = format(unit, amount, false, separators, true);
    }
    return value + QString(" ") + shortName(unit);
}

bool PeppacoinUnits::parse(Unit unit, const QString& value, CAmount* val_out)
{
    if (value.isEmpty()) {
        return false; // Refuse to parse invalid unit or empty string
    }
    int num_decimals = decimals(unit);

    // Ignore spaces and thin spaces when parsing
    QStringList parts = removeSpaces(value).split(".");

    if(parts.size() > 2)
    {
        return false; // More than one dot
    }
    QString whole = parts[0];
    QString decimals;

    if(parts.size() > 1)
    {
        decimals = parts[1];
    }
    if(decimals.size() > num_decimals)
    {
        return false; // Exceeds max precision
    }
    bool ok = false;
    QString str = whole + decimals.leftJustified(num_decimals, '0');

    if(str.size() > 18)
    {
        return false; // Longer numbers will exceed 63 bits
    }
    CAmount retvalue(str.toLongLong(&ok));
    if(val_out)
    {
        *val_out = retvalue;
    }
    return ok;
}

QString PeppacoinUnits::getAmountColumnTitle(Unit unit)
{
    return QObject::tr("Amount") + " (" + shortName(unit) + ")";
}

int PeppacoinUnits::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return unitlist.size();
}

QVariant PeppacoinUnits::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row >= 0 && row < unitlist.size())
    {
        Unit unit = unitlist.at(row);
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return QVariant(longName(unit));
        case Qt::ToolTipRole:
            return QVariant(description(unit));
        case UnitRole:
            return QVariant::fromValue(unit);
        }
    }
    return QVariant();
}

CAmount PeppacoinUnits::maxMoney()
{
    return MAX_MONEY;
}

namespace {
qint8 ToQint8(PeppacoinUnit unit)
{
    switch (unit) {
    case PeppacoinUnit::DRI: return 0;
    case PeppacoinUnit::mDRI: return 1;
    case PeppacoinUnit::uDRI: return 2;
    case PeppacoinUnit::CAC: return 3;
    } // no default case, so the compiler can warn about missing cases
    assert(false);
}

PeppacoinUnit FromQint8(qint8 num)
{
    switch (num) {
    case 0: return PeppacoinUnit::DRI;
    case 1: return PeppacoinUnit::mDRI;
    case 2: return PeppacoinUnit::uDRI;
    case 3: return PeppacoinUnit::CAC;
    }
    assert(false);
}
} // namespace

QDataStream& operator<<(QDataStream& out, const PeppacoinUnit& unit)
{
    return out << ToQint8(unit);
}

QDataStream& operator>>(QDataStream& in, PeppacoinUnit& unit)
{
    qint8 input;
    in >> input;
    unit = FromQint8(input);
    return in;
}
