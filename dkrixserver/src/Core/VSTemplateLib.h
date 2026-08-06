//////////////////////////////////////////////////////////////////////////////
// Filename    : VSTemplateLib.h
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __VSTEMPLATELIB_H__
#define __VSTEMPLATELIB_H__

#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <list>

#include <sys/time.h>

using namespace std;

template <class T> class UniqueSet {
public:
    UniqueSet(unsigned int maxSize = 100);
    ~UniqueSet();

public:
    unsigned int getMaxSize(void) {
        return m_MaxSize;
    }
    unsigned int getCurSize(void) {
        return m_CurSize;
    }

    unsigned int insert(const T value);
    unsigned int pick(unsigned int size, list<T>& resultList);
    unsigned int pick(unsigned int size, vector<T>& resultVector);

protected:
    unsigned int m_MaxSize;
    unsigned int m_CurSize;
    T* m_pData;
};

template <class T> UniqueSet<T>::UniqueSet(unsigned int maxSize) {
    m_pData = new T[maxSize];
    m_MaxSize = maxSize;
    m_CurSize = 0;
}

template <class T> UniqueSet<T>::~UniqueSet() {
    if (m_pData != NULL) {
        delete[] m_pData;
        m_pData = NULL;
    }
}

template <class T> unsigned int UniqueSet<T>::insert(const T value) {
    if (m_CurSize >= m_MaxSize) {
        cerr << "UniqueSet::insert() : Max Size Exceeded" << endl;
        return m_MaxSize + 1;
    }

    m_pData[m_CurSize] = value;
    m_CurSize++;

    return m_CurSize;
}

template <class T> unsigned int UniqueSet<T>::pick(unsigned int size, list<T>& resultList) {
    
    
    if (size >= m_CurSize) {
        for (unsigned int i = 0; i < m_CurSize; i++) {
            resultList.push_back(m_pData[i]);
        }
    }
    
    
    else {
        
        
        
        
        
        
        int max = m_CurSize;
        int current = 0;

        while (current < size) {
            int index = rand() % max;

            resultList.push_back(m_pData[index]);

            T temp = m_pData[index];
            m_pData[index] = m_pData[max - 1];
            m_pData[max - 1] = temp;

            max--;
            current++;
        }
    }

    return resultList.size();
}

template <class T> unsigned int UniqueSet<T>::pick(unsigned int size, vector<T>& resultVector) {
    
    
    if (size >= m_CurSize) {
        for (unsigned int i = 0; i < m_CurSize; i++) {
            resultVector.push_back(m_pData[i]);
        }
    }
    
    
    else {
        
        
        
        
        
        
        int max = m_CurSize;
        int current = 0;

        while (current < size) {
            int index = rand() % max;

            resultVector.push_back(m_pData[index]);

            T temp = m_pData[index];
            m_pData[index] = m_pData[max - 1];
            m_pData[max - 1] = temp;

            max--;
            current++;
        }
    }

    return resultVector.size();
}

#endif
