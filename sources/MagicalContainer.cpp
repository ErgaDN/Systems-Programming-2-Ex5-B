#include "MagicalContainer.hpp"
namespace ariel

{
    ////////// MagicalContainer class //////////
    void MagicalContainer::addElement(int element)
    {
        auto itr = std::lower_bound(getVec().begin(), getVec().end(), element);

        if (itr != getVec().end() && *itr == element)
        {
            return;
        }

        itr = getVec().insert(itr, element);

        if (isPrime(element))
        {
            int *ptr = new int(element); // Create a pointer to an int with the value of element
            auto ptrIt = std::lower_bound(getPrime().begin(), getPrime().end(), ptr,
                                          [](const int *aIdx, const int *bIdx)
                                          {
                                              return *aIdx < *bIdx;
                                          });
            getPrime().insert(ptrIt, ptr);
        }
    }

    void MagicalContainer::removeElement(int element)
    {
        auto vecIt = std::remove(getVec().begin(), getVec().end(), element);
        if (vecIt == getVec().end())
        {
            throw std::runtime_error("Element not found");
        }
        getVec().erase(vecIt, getVec().end());

        // Remove the pointer to the int element from pointerVec
        getPrime().erase(std::remove_if(getPrime().begin(), getPrime().end(), [&](const int *ptr)
                                        { return (*ptr) == element; }),
                         getPrime().end());
    }

    bool MagicalContainer::isPrime(int number)
    {
        if (number < 2)
        {
            return false;
        }
        for (int i = 2; i <= std::sqrt(number); ++i)
        {
            if (number % i == 0)
            {
                return false;
            }
        }
        return true;
    }

    bool MagicalContainer::iterator::operator==(const iterator &other) const
    {
        if (typeid(*this) != typeid(other))
        {
            throw std::runtime_error("Incompatible iterator types");
        }
        return getIndex() == other.getIndex() && getBeginSide() == other.getBeginSide();
    }

    MagicalContainer::iterator &MagicalContainer::iterator::operator=(const iterator &other)
    {
        if (&_container != &other._container)
        {
            throw std::runtime_error("Incompatible iterator types");
        }

        if (this != &other)
        {
            setIndex(other.getIndex());
        }

        return *this;
    }

    bool MagicalContainer::iterator::operator!=(const iterator &other) const
    {
        return !(*this == other);
    }

    bool MagicalContainer::iterator::operator>(const iterator &other) const
    {
        return getIndex() > other.getIndex() || (getIndex() == other.getIndex() && getBeginSide() != other.getBeginSide());
    }

    bool MagicalContainer::iterator::operator<(const iterator &other) const
    {
        return !(*this > other) && *this != other;
    }

    ////////// AscendingIterator class //////////
    int MagicalContainer::AscendingIterator::operator*()
    {
        return getContainer().getVec()[static_cast<vector<int>::size_type>(getIndex())];
    }

    MagicalContainer::AscendingIterator &MagicalContainer::AscendingIterator::operator++()
    {
        if (this->getIndex() == this->getContainer().getVec().size())
        {
            throw runtime_error("iterator at the end-1");
        }

        this->setIndex(this->getIndex() + 1);
        return *this;
    }

    MagicalContainer::AscendingIterator &MagicalContainer::AscendingIterator::begin()
    {
        this->setIndex(0);
        return *this;
    }

    MagicalContainer::AscendingIterator &MagicalContainer::AscendingIterator::end()
    {
        this->setIndex(this->getContainer().getVec().size());
        return *this;
    }

    ////////// SideCrossIterator class //////////
    int MagicalContainer::SideCrossIterator::operator*()
    {
        if (getBeginSide())
        {
            return getContainer().getVec()[getIndex()];
        }

        return getContainer().getVec()[getContainer().size() - 1 - getIndex()];
    }

    MagicalContainer::SideCrossIterator &MagicalContainer::SideCrossIterator::operator++()
    {
        size_t cntSize = getContainer().size();
        if (getIndex() == cntSize)
        {
            throw runtime_error("iterator at the end-2");
        }

        if (cntSize % 2 == 0)
        {
            if (!getBeginSide())
            {
                (getIndex() == cntSize / 2 - 1) ? setIndex(cntSize) : setIndex(getIndex() + 1);
            }
        }
        else // (cntSize % 2 == 1)
        {
            if (getBeginSide())
            {
                if (getIndex() == cntSize / 2)
                {
                    setIndex(cntSize);
                }
            }
            else
            {
                setIndex(getIndex() + 1);
            }
        }
        setBeginSide(!getBeginSide());
        return *this;
    }

    MagicalContainer::SideCrossIterator &MagicalContainer::SideCrossIterator::begin()
    {
        this->setIndex(0);
        this->setBeginSide(true);
        return *this;
    }

    MagicalContainer::SideCrossIterator &MagicalContainer::SideCrossIterator::end()
    {

        this->setIndex(getContainer().size());
        setBeginSide(false);
        return *this;
    }

    ////////// PrimeIterator class //////////
    int MagicalContainer::PrimeIterator::operator*()
    {
        return *(getContainer().getPrime()[getIndex()]);
    }

    MagicalContainer::PrimeIterator &MagicalContainer::PrimeIterator::operator++()
    {
        if (getIndex() == getContainer().getPrime().size())
        {
            throw runtime_error("increment beyond the end");
        }
        setIndex(getIndex() + 1);
        return *this;
    }

    MagicalContainer::PrimeIterator &MagicalContainer::PrimeIterator::begin()
    {
        setIndex(0);
        return *this;
    }

    MagicalContainer::PrimeIterator &MagicalContainer::PrimeIterator::end()
    {

        setIndex(getContainer().getPrime().size());
        return *this;
    }
}