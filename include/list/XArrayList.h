/*
 * File:   XArrayList.h
 */

#ifndef XARRAYLIST_H
#define XARRAYLIST_H
#include "list/IList.h"
#include <memory.h>
#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class XArrayList : public IList<T>
{
public:
    class Iterator; // forward declaration

protected:
    T *data;                                 // dynamic array to store the list's items
    int capacity;                            // size of the dynamic array
    int count;                               // number of items stored in the array
    bool (*itemEqual)(T &lhs, T &rhs);       // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(XArrayList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    XArrayList(
        void (*deleteUserData)(XArrayList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0,
        int capacity = 10);
    XArrayList(const XArrayList<T> &list);
    XArrayList<T> &operator=(const XArrayList<T> &list);
    ~XArrayList();

    // Inherit from IList: BEGIN
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IList: BEGIN

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(XArrayList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    Iterator begin()
    {
        return Iterator(this, 0);
    }
    Iterator end()
    {
        return Iterator(this, count);
    }

    static void free(XArrayList<T> *list)
    {
        typename XArrayList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

protected:
    void checkIndex(int index);     // check validity of index for accessing
    void ensureCapacity(int index); // auto-allocate if needed

    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }

    void copyFrom(const XArrayList<T> &list);

    void removeInternalData();

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    // Iterator: BEGIN
    class Iterator
    {
    private:
        int cursor;
        XArrayList<T> *pList;

    public:
        Iterator(XArrayList<T> *pList = 0, int index = 0)
        {
            this->pList = pList;
            this->cursor = index;
        }
        Iterator &operator=(const Iterator &iterator)
        {
            cursor = iterator.cursor;
            pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            T item = pList->removeAt(cursor);
            if (removeItemData != 0)
                removeItemData(item);
            cursor -= 1; // MUST keep index of previous, for ++ later
        }

        T &operator*()
        {
            return pList->data[cursor];
        }
        bool operator!=(const Iterator &iterator)
        {
            return cursor != iterator.cursor;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            this->cursor++;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
    // Iterator: END
};

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
XArrayList<T>::XArrayList(
    void (*deleteUserData)(XArrayList<T> *),
    bool (*itemEqual)(T &, T &),
    int capacity)
{
    this->deleteUserData = deleteUserData;
    this->itemEqual = itemEqual;
    this->capacity = capacity;
    this->count = 0;
    data = new T[capacity];
}

template <class T>
void XArrayList<T>::copyFrom(const XArrayList<T> &list)
{
    this->removeInternalData();

    capacity = list.capacity;
    count = list.count;
    itemEqual = list.itemEqual;
    deleteUserData = list.deleteUserData;

    data = new T[capacity];

    for (int i = 0; i < count; i++)
    {
        data[i] = list.data[i];
    }
}

template <class T>
void XArrayList<T>::removeInternalData()
{
    if (deleteUserData)
    {
        deleteUserData(this);
    }

    delete[] data;
    data = nullptr;

    count = 0;
    capacity = 0;
}

template <class T>
XArrayList<T>::XArrayList(const XArrayList<T> &list)
{
    capacity = list.capacity;
    count = list.count;
    itemEqual = list.itemEqual;
    deleteUserData = list.deleteUserData;

    data = new T[capacity];
    for (int i = 0; i < count; i++)
    {
        data[i] = list.data[i];
    }
}

template <class T>
XArrayList<T> &XArrayList<T>::operator=(const XArrayList<T> &list)
{
    if (this != &list)
    {
        delete[] data;
        capacity = list.capacity;
        count = list.count;
        itemEqual = list.itemEqual;
        deleteUserData = list.deleteUserData;
        data = new T[capacity];
        for (int i = 0; i < count; ++i)
        {
            data[i] = list.data[i];
        }
    }
    return *this;
}

template <class T>
XArrayList<T>::~XArrayList()
{
    if (deleteUserData)
    {
        deleteUserData(this);
    }
    delete[] data;
}

template <class T>
void XArrayList<T>::add(T e)
{
    if (count == capacity)
    {
        capacity = capacity * 2;
        T *newData = new T[capacity];
        for (int i = 0; i < count; i++)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }
    data[count] = e;
    count++;
}

template <class T>
void XArrayList<T>::add(int index, T e)
{
    if (index < 0 || index > count)
    {
        throw out_of_range("Index is out of range!");
    }
    if (count == capacity)
    {
        capacity = capacity * 2;
        T *newData = new T[capacity];
        for (int i = 0; i < count; i++)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }
    for (int i = count; i > index; i--)
    {
        data[i] = data[i - 1];
    }
    data[index] = e;
    count++;
}

template <class T>
T XArrayList<T>::removeAt(int index)
{
    if (index < 0 || index >= count)
    {
        throw out_of_range("Index is out of range!");
    }
    T removedData = data[index];
    for (int i = index; i < count - 1; i++)
    {
        data[i] = data[i + 1];
    }
    count--;
    return removedData;
}

template <class T>
bool XArrayList<T>::removeItem(T item, void (*removeItemData)(T))
{

    for (int i = 0; i < count; i++)
    {
        // if(data[i] == item){
        //     if (removeItemData != nullptr) {
        //         removeItemData(data[i]);
        //     } else {
        //         removeAt(i);

        //         return true;
        //     }
        // }

        if (equals(data[i], item, itemEqual))
        {
            removeAt(i);
            return true;
        }
    }
    return false;
}

template <class T>
bool XArrayList<T>::empty()
{
    if (count == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <class T>
int XArrayList<T>::size()
{
    return count;
}

template <class T>
void XArrayList<T>::clear()
{
    delete[] data;
    count = 0;
    capacity = 10;
    data = new T[capacity];
}

template <class T>
T &XArrayList<T>::get(int index)
{
    if (index < 0 || index >= count)
    {
        throw std::out_of_range("Index is out of range!");
    }
    return data[index];
}

template <class T>
int XArrayList<T>::indexOf(T item)
{
    for (int i = 0; i < count; i++)
    {
        // if(data[i] == item){
        //     return i;
        // }
        if (equals(data[i], item, itemEqual))
        {
            return i;
        }
    }
    return -1;
}
template <class T>
bool XArrayList<T>::contains(T item)
{
    if (indexOf(item) != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <class T>
string XArrayList<T>::toString(string (*item2str)(T &))
{
    stringstream ss;
    ss << "[";
    for (int i = 0; i < count; i++)
    {
        if (i > 0)
        {
            ss << ", ";
        }
        if (item2str != nullptr)
        {
            ss << item2str(data[i]);
        }
        else
        {
            ss << data[i];
        }
    }
    ss << "]";
    return ss.str();
}

//////////////////////////////////////////////////////////////////////
//////////////////////// (private) METHOD DEFNITION //////////////////
//////////////////////////////////////////////////////////////////////
template <class T>
void XArrayList<T>::checkIndex(int index)
{
    if (index < 0 || index >= count)
    {
        throw out_of_range("Index is out of range!");
    }
}
template <class T>
void XArrayList<T>::ensureCapacity(int index)
{
    if (index < 0)
    {
        throw out_of_range("Index is out of range!");
    }
    if (index >= capacity)
    {
        int newCapacity = capacity;
        while (newCapacity <= index)
        {
            newCapacity = newCapacity * 2;
        }
        T *newData = new T[newCapacity];

        for (int i = 0; i < count; i++)
        {
            newData[i] = data[i];
        }

        delete[] data;

        data = newData;
        capacity = newCapacity;
    }
}

#endif /* XARRAYLIST_H */