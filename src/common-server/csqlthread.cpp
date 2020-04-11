
#include <windows.h>

#include "CSqlTHREAD.h"

#include "classUTIL.h"

#include "rose/common/log.h"
#include "rose/network/packet.h"

#include <queue>

CSqlTHREAD::CSqlTHREAD(bool bCreateSuspended): classTHREAD(bCreateSuspended), m_CS(4000) {
    m_pEVENT = new classEVENT(NULL, false, false, NULL);
    m_pSQL = NULL;
}
CSqlTHREAD::~CSqlTHREAD() {
    this->Free();

    SAFE_DELETE(m_pEVENT);
    SAFE_DELETE(m_pSQL);
}

bool
CSqlTHREAD::Connect(BYTE btSqlTYPE,
    char* szServerIP,
    char* szUser,
    char* szPassword,
    char* szDBName,
    short nBindParamCNT,
    WORD wQueryBufferLEN) {
    SAFE_DELETE(m_pSQL);

    char* szDNSorIP;
    switch (btSqlTYPE) {
        case USE_ODBC: {
            classODBC* pODBC = new classODBC(nBindParamCNT, wQueryBufferLEN);
            if (!pODBC->ReigsterDsnIfNone(szDBName, szDBName, szServerIP, szUser))
                return false;

            m_pSQL = pODBC;
            szDNSorIP = szDBName;
            break;
        }
            // case USE_MY_SQL :
            //	m_pSQL = new classMYSQL(nBindParamCNT, wQueryBufferLEN);
            //	szDNSorIP = szServerIP;
            //	break;
    }
    if (!m_pSQL)
        return false;

    if (!m_pSQL->Connect(szDNSorIP, szUser, szPassword)) {
        SAFE_DELETE(m_pSQL);
        return false;
    }
    if (!m_pSQL->SelectDB(szDBName)) {
        m_pSQL->Disconnect();
        SAFE_DELETE(m_pSQL);
        return false;
    }

    return true;
}

void
CSqlTHREAD::Free() {
    this->Terminate();
    do {
        this->m_pEVENT->SetEvent();
    } while (!IsFinished());
}

CDLList<tagQueryDATA>::tagNODE*
CSqlTHREAD::Del_SqlPACKET(CDLList<tagQueryDATA>::tagNODE* pDelNODE) {
    CDLList<tagQueryDATA>::tagNODE* pNextNODE;

    pNextNODE = m_RunPACKET.GetNextNode(pDelNODE);

    SAFE_DELETE_ARRAY(pDelNODE->m_VALUE.m_pDATA);
    m_RunPACKET.DeleteNFree(pDelNODE);

    return pNextNODE;
}

bool
CSqlTHREAD::Add_SqlPACKET(int iTAG, char* szName, BYTE* pDATA, int iDataSize) {
    CDLList<tagQueryDATA>::tagNODE* pNewNODE;

    pNewNODE = new CDLList<tagQueryDATA>::tagNODE;
    if (!pNewNODE) {
        // out of memory !!!
        return false;
    }

    pNewNODE->m_VALUE.m_iTAG = iTAG;
    pNewNODE->m_VALUE.m_Name.Set(szName);
    pNewNODE->m_VALUE.m_iDataLEN = iDataSize;
    pNewNODE->m_VALUE.m_pDATA = new BYTE[iDataSize];
    if (!pNewNODE->m_VALUE.m_pDATA) {
        // out of memory !!!
        SAFE_DELETE(pNewNODE);
        return false;
    }

    ::CopyMemory(pNewNODE->m_VALUE.m_pDATA, pDATA, iDataSize);

    m_CS.Lock();
    m_AddPACKET.AppendNode(pNewNODE);
    m_CS.Unlock();
    m_pEVENT->SetEvent();

    return true;
}

bool
CSqlTHREAD::Add_QueryString(char* szQuery) {
    CDLList<char*>::tagNODE* pNewNODE;

    pNewNODE = new CDLList<char*>::tagNODE;
    if (!pNewNODE) {
        // out of memory !!!
        return false;
    }

    int iStrLen = strlen(szQuery);
    pNewNODE->m_VALUE = new char[iStrLen + 1];
    ::CopyMemory(pNewNODE->m_VALUE, szQuery, iStrLen);
    pNewNODE->m_VALUE[iStrLen] = 0;

    m_CS.Lock();
    m_AddQUERY.AppendNode(pNewNODE);
    m_CS.Unlock();
    m_pEVENT->SetEvent();

    return true;
}

bool
CSqlTHREAD::Proc_QuerySTRING() {
    this->m_CS.Lock();
    m_RunQUERY.AppendNodeList(&m_AddQUERY);
    m_AddQUERY.Init();
    this->m_CS.Unlock();

    CDLList<char*>::tagNODE *pNode, *pDel;
    pNode = m_RunQUERY.GetHeadNode();
    while (pNode) {
        if (this->m_pSQL->ExecSQLBuffer(pNode->m_VALUE)) {
            pDel = pNode;
            pNode = pNode->GetNext();

            SAFE_DELETE_ARRAY(pDel->m_VALUE);
            m_RunQUERY.DeleteNFree(pDel);
        } else
            pNode = pNode->GetNext();
    }
    return true;
}

void
CSqlTHREAD::tick() {
    std::lock_guard<std::mutex> lock(this->queue_mutex);
    while (!this->packet_queue.empty()) {
        this->handle_queued_packet(this->packet_queue.front());
        this->packet_queue.pop();
    }
}

void
CSqlTHREAD::queue_packet(int32_t socket_id,
    const std::string& account_name,
    const Rose::Network::Packet& p) {

    const std::lock_guard<std::mutex> lock(this->queue_mutex);
    this->packet_queue.push({ socket_id, account_name, p });
    m_pEVENT->SetEvent();
}
