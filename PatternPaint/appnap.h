// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef APPNAP_H
#define APPNAP_H

class CAppNapInhibitor
{
public:
    CAppNapInhibitor(const char* strReason);
    ~CAppNapInhibitor();
private:
    class Private;
    Private* d;
};

#endif // APPNAP_H

