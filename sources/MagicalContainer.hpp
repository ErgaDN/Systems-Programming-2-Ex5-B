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
        ~MagicalContainer() {
            // Release memory for the int* pointers in _prime vector
            for (auto ptr : _prime) {
                delete ptr;
            }
        }

        void addElement(int element)
        {
            auto it = std::lower_bound(getVec().begin(), getVec().end(), element);

            if (it == getVec().end() || *it != element)
            {
                getVec().insert(it, element);
            }

            if (isPrime(element))
            {
                int *ptr = new int(element); // Create a pointer to an int with the value of element
                auto ptrIt = std::lower_bound(getPrime().begin(), getPrime().end(), ptr,
                                              [](const int *a, const int *b)
                                              {
                                                  return *a < *b;
                                              });
                getPrime().insert(ptrIt, ptr);
            }
        }

        void removeElement(int element)
        {
            // getVec().erase(std::remove(getVec().begin(), getVec().end(), element), getVec().end());
            auto vecIt = std::remove(getVec().begin(), getVec().end(), element);
            if (vecIt == getVec().end()) {
                throw std::runtime_error("Element not found");
            }
            getVec().erase(vecIt, getVec().end());

            // Remove the pointer to the int element from pointerVec
            getPrime().erase(std::remove_if(getPrime().begin(), getPrime().end(), [&](int *ptr)
                                            { return (*ptr) == element; }),
                             getPrime().end());
        }

        size_t size() const // size_t ??
        {
            return _elements.size();
        }

        vector<int> &getVec() { return _elements; }
        vector<int *> &getPrime() { return _prime; }
        // int getElem() { return }

        // bool operator==(const MagicalContainer &other) const
        // {
        //     return _elements == other._elements && _prime == _prime;
        // }

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
        protected:
            MagicalContainer &_container;
            size_t _index;
            bool _beginSide; // True for begin() side, False for end() side

        public:
            iterator(MagicalContainer &container) : _container(container), _index(0), _beginSide(true) {}
            iterator(iterator &other) : _container(other.getContainer()), _index(getIndex()), _beginSide(getBeginSide()) {}
            ~iterator() = default;

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

            iterator& operator=(const iterator& other) 
            {
                if (&_container != &other._container)
                    throw std::runtime_error("Incompatible iterator types");
                // if (typeid(*this) != typeid(other)) {
                //     throw std::runtime_error("Incompatible iterator types");
                // }

                if (this != &other) {
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
                return getContainer().getVec()[getIndex()];
            }

            AscendingIterator &operator++() override
            {
                if (getIndex() == getContainer().size())
                    throw runtime_error("iterator at the end");

                this->setIndex(getIndex() + 1);
                return *this;
            }

            AscendingIterator &begin() override
            {
                this->setIndex(0);
                return *this;
            }

            AscendingIterator &end() override
            {
                // if (getIndex() == getContainer().getVec().size())
                //     throw runtime_error("increment beyond the end");
                this->setIndex(getContainer().size());
                return *this;
            }

            // bool operator==(const AscendingIterator &other) const
            // {
            //     return _index == other._index;
            // }

            // bool operator>(const AscendingIterator &other) const
            // {
            //     return _index > other._index;
            // }

            // bool operator<(const AscendingIterator &other) const
            // {
            //     return _index < other._index;
            // }

            // int operator*() const
            // {
            //     if (_index >= _container.size())
            //     {
            //         throw std::out_of_range("Iterator out of range");
            //     }
            //     return _container._elements[_index];
            // }

            // AscendingIterator &operator++()
            // {
            //     ++_index;
            //     return *this;
            // }

            // AscendingIterator begin() const
            // {
            //     return AscendingIterator(_container);
            // }

            // AscendingIterator end() const
            // {
            //     AscendingIterator iter(_container);
            //     iter._index = _container.size();
            //     return iter;
            // }
        };

        class SideCrossIterator : public iterator
        {
        public:
            SideCrossIterator(MagicalContainer &container) : iterator(container) {}

            int operator*() override
            {
                if (getBeginSide())
                    return getContainer().getVec()[getIndex()];
                else
                    return getContainer().getVec()[getContainer().size() - 1 - getIndex()];
            }

            SideCrossIterator &operator++() override
            {
                size_t cntSize = getContainer().size();
                if (getIndex() == cntSize)
                    throw runtime_error("iterator at the end");

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
                // unsigned int cntSize = getContainer().size();
                // this->setIndex(cntSize - 1);
                // if (cntSize % 2 == 0)
                // {

                // }
                
                this->setIndex(getContainer().size());
                setBeginSide(false);
                return *this;
                // this->setIndex(getContainer().size());
                // return *this;
            }

            //     const MagicalContainer &container;
            //     unsigned int currentIndex;
            //     bool reverse;

            // public:
            //     SideCrossIterator(const MagicalContainer &cont, bool rev = false)
            //         : container(cont), currentIndex(0), reverse(rev) {}

            //     bool operator!=(const SideCrossIterator &other) const
            //     {
            //         return currentIndex != other.currentIndex;
            //     }

            //     bool operator==(const SideCrossIterator &other) const
            //     {
            //         return currentIndex == other.currentIndex;
            //     }

            //     bool operator>(const SideCrossIterator &other) const
            //     {
            //         return currentIndex > other.currentIndex;
            //     }

            //     bool operator<(const SideCrossIterator &other) const
            //     {
            //         return currentIndex < other.currentIndex;
            //     }

            //     int operator*() const
            //     {
            //         if (currentIndex >= container.size())
            //         {
            //             throw std::out_of_range("Iterator out of range");
            //         }
            //         return container._elements[currentIndex];
            //     }

            //     SideCrossIterator &operator++()
            //     {
            //         if (reverse)
            //         {
            //             --currentIndex;
            //         }
            //         else
            //         {
            //             ++currentIndex;
            //         }
            //         return *this;
            //     }

            //     SideCrossIterator begin() const
            //     {
            //         return SideCrossIterator(container, false);
            //     }

            //     SideCrossIterator end() const
            //     {
            //         SideCrossIterator iter(container, true);
            //         iter.currentIndex = container.size() - 1;
            //         return iter;
            //     }
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
                    throw runtime_error("increment beyond the end");
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

            //     const MagicalContainer &container;
            //     unsigned int currentIndex;

            //     static bool isPrime(int number)
            //     {
            //         if (number < 2)
            //         {
            //             return false;
            //         }
            //         for (int i = 2; i <= std::sqrt(number); ++i)
            //         {
            //             if (number % i == 0)
            //             {
            //                 return false;
            //             }
            //         }
            //         return true;
            //     }

            // public:
            //     PrimeIterator(const MagicalContainer &cont) : container(cont), currentIndex(0)
            //     {
            //         while (currentIndex < container.size() && !isPrime(container._elements[currentIndex]))
            //         {
            //             ++currentIndex;
            //         }
            //     }

            //     bool operator!=(const PrimeIterator &other) const
            //     {
            //         return currentIndex != other.currentIndex;
            //     }

            //     bool operator==(const PrimeIterator &other) const
            //     {
            //         return currentIndex == other.currentIndex;
            //     }

            //     bool operator>(const PrimeIterator &other) const
            //     {
            //         return currentIndex > other.currentIndex;
            //     }

            //     bool operator<(const PrimeIterator &other) const
            //     {
            //         return currentIndex < other.currentIndex;
            //     }

            //     int operator*() const
            //     {
            //         if (currentIndex >= container.size())
            //         {
            //             throw std::out_of_range("Iterator out of range");
            //         }
            //         return container._elements[currentIndex];
            //     }

            //     PrimeIterator &operator++()
            //     {
            //         ++currentIndex;
            //         while (currentIndex < container.size() && !isPrime(container._elements[currentIndex]))
            //         {
            //             ++currentIndex;
            //         }
            //         return *this;
            //     }

            //     PrimeIterator begin() const
            //     {
            //         return PrimeIterator(container);
            //     }

            //     PrimeIterator end() const
            //     {
            //         PrimeIterator iter(container);
            //         iter.currentIndex = container.size();
            //         return iter;
            //     }
        };
    };
}
#endif