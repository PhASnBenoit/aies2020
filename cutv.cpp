#include "cutv.h"

CUTv::CUTv(QObject *parent) : QObject(parent)
{
    mUTv = new CGpio(UTV);
}

CUTv::~CUTv()
{
    delete mUTv;
}

bool CUTv::getU()
{
    mValU = mUTv->lire();
    return mValU;
}
