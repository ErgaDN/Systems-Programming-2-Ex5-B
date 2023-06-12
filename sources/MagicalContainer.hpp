#ifndef MAGICALCONTAINER_HPP
#define MAGICALCONTAINER_HPP
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>

using namespace std;

namespace ariel
{
    class MagicalContainer
    {
        vector<int> _elements;
        vector<int *> _prime;

    public:
        MagicalContainer() {}
        // Disable copy constructor
        MagicalContainer(const MagicalContainer &) = delete;

        // Disable copy assignment operator
        MagicalContainer &operator=(const MagicalContainer &) = delete;
        // Disable move constructor
        MagicalContainer(MagicalContainer &&) = delete;

        // Disable move assignment operator
        MagicalContainer &operator=(MagicalContainer &&) = delete;
        ~MagicalContainer()
        {
            // Release memory for the int* pointers in _prime vector
            for (auto *ptr : _prime)
            {
                delete ptr;
            }
        }

        void addElement(int element)
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

        void removeElement(int element)
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

        size_t size() const
        {
            return _elements.size();
        }

        vector<int> &getVec() { return _elements; }
        vector<int *> &getPrime() { return _prime; }

        static bool isPrime(int number)
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

        class iterator
        {
        private:
            MagicalContainer &_container;
            size_t _index;
            bool _beginSide; // True for begin() side, False for end() side

        public:
            iterator(MagicalContainer &container) : _container(container), _index(0), _beginSide(true) {}
            iterator(iterator &other) : _container(other.getContainer()), _index(other.getIndex()), _beginSide(other.getBeginSide()) {}
            virtual ~iterator() = default;
            // Disable move constructor
            iterator(iterator &&) = delete;

            // Disable move assignment operator
            iterator &operator=(iterator &&) = delete;

            MagicalContainer &getContainer() { return _container; }
            size_t getIndex() const { return _index; }
            bool getBeginSide() const { return _beginSide; }

            void setIndex(size_t idx) { _index = idx; }
            void setBeginSide(bool boolean) { _beginSide = boolean; }

            bool operator==(const iterator &other) const
            {
                if (typeid(*this) != typeid(other))
                {
                    throw std::runtime_error("Incompatible iterator types");
                }
                return getIndex() == other.getIndex() && getBeginSide() == other.getBeginSide();
            }

            iterator &operator=(const iterator &other)
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

            bool operator!=(const iterator &other) const
            {
                return !(*this == other);
            }

            bool operator>(const iterator &other) const
            {
                return getIndex() > other.getIndex() || (getIndex() == other.getIndex() && getBeginSide() != other.getBeginSide());
            }

            bool operator<(const iterator &other) const
            {
                return !(*this > other) && *this != other;
            }

            virtual int operator*() = 0;

            virtual iterator &operator++() = 0;

            virtual iterator &begin() = 0;

            virtual iterator &end() = 0;
        };

        class AscendingIterator : public iterator
        {
        public:
            AscendingIterator(MagicalContainer &container) : iterator(container) {}

            int operator*() override
            {
                return getContainer().getVec()[static_cast<vector<int>::size_type>(getIndex())];
            }

            AscendingIterator &operator++() override
            {
                if (this->getIndex() == this->getContainer().getVec().size())
                {
                    throw runtime_error("iterator at the end-1");
                }

                this->setIndex(this->getIndex() + 1);
                return *this;
            }

            AscendingIterator &begin() override
            {
                this->setIndex(0);
                return *this;
            }

            AscendingIterator &end() override
            {
                this->setIndex(this->getContainer().getVec().size());
                return *this;
            }
        };

        class SideCrossIterator : public iterator
        {
        public:
            SideCrossIterator(MagicalContainer &container) : iterator(container) {}

            int operator*() override
            {
                if (getBeginSide())
                {
                    return getContainer().getVec()[getIndex()];
                }

                return getContainer().getVec()[getContainer().size() - 1 - getIndex()];
            }

            SideCrossIterator &operator++() override
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

            SideCrossIterator &begin() override
            {
                this->setIndex(0);
                this->setBeginSide(true);
                return *this;
            }

            SideCrossIterator &end() override
            {

                this->setIndex(getContainer().size());
                setBeginSide(false);
                return *this;
            }
        };

        class PrimeIterator : public iterator
        {
        public:
            PrimeIterator(MagicalContainer &container) : iterator(container) {}

            int operator*() override
            {
                return *(getContainer().getPrime()[getIndex()]);
            }

            PrimeIterator &operator++() override
            {
                if (getIndex() == getContainer().getPrime().size())
                {
                    throw runtime_error("increment beyond the end");
                }
                setIndex(getIndex() + 1);
                return *this;
            }

            PrimeIterator &begin() override
            {
                setIndex(0);
                return *this;
            }

            PrimeIterator &end() override
            {

                setIndex(getContainer().getPrime().size());
                return *this;
            }
        };
    };
}
#endif