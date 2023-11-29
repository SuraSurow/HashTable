#include <iostream>
#include <random>


const int INTMAX = 100;//2147483647   2 000 000 000


std::mt19937 randomEngine;
/**
 * Generuje losową liczbę całkowitą z podanego zakresu.
 * @param min Minimalna wartość losowanej liczby.
 * @param max Maksymalna wartość losowanej liczby.
 * @return Wygenerowana liczba całkowita.
 */
int randValue(int min, int max)
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(randomEngine);
}

void initRandomEngine();
/**
 * Inicjalizuje generator liczb losowych.
 */
void initRandomEngine()
{
    std::random_device rd;
    std::seed_seq seed{rd(), rd(), rd(), rd()};
    randomEngine.seed(seed);
}


/**
 * Struktura reprezentująca obiekt O, zawierająca losową liczbę.
 */
struct O
{
    int number;
    O() : number(randValue(0, INTMAX)) {}
    int operator()() const {
        return number;
    }
};
/**
 * Porównuje dwa obiekty typu O pod względem ich losowych liczb.
 * @param a Pierwszy obiekt.
 * @param b Drugi obiekt.
 * @return 1, jeśli a > b; 0, jeśli a == b; -1, jeśli a < b.
 */
int static compareObj(O a, O b)
{
    if (a.number > b.number)
        return 1;
    else if (a.number == b.number)
        return 0;
    else
        return -1;
}


/**
 * Generuje losowy klucz o określonej długości.
 * @param size Długość generowanego klucza.
 * @return Wygenerowany klucz.
 */
std::string generateKey(unsigned int size)
{
    std::string ret;
    for (int i = 0 ; i < size ; i++)
    {
        ret+= static_cast<char>(randValue(65,90));
    }
    return ret;
}



template <typename V>
class Dynamic_Array
{
protected:
    class LinkedList {
    public:
        struct Node {
            std::string key;
            V value;
            Node *next;
            Node *previous;
            Node ( const std::string &k, const V &v ) : key(k), value(v), next(nullptr), previous(nullptr) {}
        };
        Node *first;
        Node *last;
        LinkedList ( ) : first(nullptr), last(nullptr) {}
        void push_back ( const std::string &key, const V &value ) {

            Node *newNode = new Node(key, value);
            if ( !first ) {
                first = last = newNode;

            }
            else {
                last->next = newNode;
                newNode->previous = last;
                last = newNode;
            }
        }
        void pop_back ( ) {
            if ( !last ) {
                std::cerr << "Lista jest pusta." << std::endl;
                return;
            }
            Node *temp = last;
            last = last->previous;
            if ( last ) {
                last->next = nullptr;
            } else {
                first = nullptr;
            }
            delete temp;
        }
        std::string toString(Node* node)
        {
            Node * current = node;
            std::string ret;
            ret += "Key =\t";
            ret += current->key;
            ret += "\tValue =\t";
            ret += std::to_string(current->value());
            while ( current->next )
            {
                current = current->next;
                ret += "\n\t   Key =\t";
                ret += current->key;
                ret += "\tValue =\t";
                ret += std::to_string(current->value());
            }
            return  ret;
        }
        Node *find ( const std::string &key ) const {
            Node *current = first;
            while ( current ) {
                if ( current->key == key ) {
                    return current;
                }
                else
                {

                }
                current = current->next;
            }
            return nullptr;
        }
        bool remove ( const std::string &key ) {
            Node *nodeToRemove = find(key);

            if ( !nodeToRemove ) {
                std::cerr << "Element nie znaleziony." << std::endl;
                return false;
            }

            if ( nodeToRemove->previous ) {
                nodeToRemove->previous->next = nodeToRemove->next;
            } else {
                first = nodeToRemove->next;
            }

            if ( nodeToRemove->next ) {
                nodeToRemove->next->previous = nodeToRemove->previous;
            } else {
                last = nodeToRemove->previous;
            }

            delete nodeToRemove;
            return true;
        }
        void clear ( ) {
            while ( first ) {
                Node *temp = first;
                first = first->next;
                delete temp;
            }
            last = nullptr;
        }
    };
public:
    LinkedList *array;
    unsigned int currSize;
    unsigned int maxSize;
    const unsigned int growth = 2;
    const double maxFillingPrecentage = 0.75;

    unsigned int hash(const std::string &key,unsigned int size) const
    {
        const unsigned int base = 31;
        unsigned int result = 0;

        for (int i = 0; i < key.size(); i++)
        {
            result = (result * base + key[i]) % size;
        }

        return result;
    }

    Dynamic_Array() : currSize(0), maxSize(7) { array = new LinkedList[maxSize]; }
    ~Dynamic_Array() { delete[] array; }
/**
 * Znajduje węzeł o podanym kluczu w tablicy mieszającej.
 * @param key Klucz do znalezienia.
 * @return Wskaźnik na znaleziony węzeł lub nullptr, jeśli nie znaleziono.
 */
    typename LinkedList::Node* find(const std::string &key) {
        unsigned int index = hash(key, maxSize);
        LinkedList &listAtIndex = array[index];

        typename LinkedList::Node *currentNode = listAtIndex.first;

        while (currentNode != nullptr) {
            if (currentNode->key == key) {
                // Znaleziono element, zwróć wskaźnik na węzeł
                return currentNode;
            }
            currentNode = currentNode->next;
        }

        // Element nie został znaleziony
        return nullptr;
    }
/**
 * Zwraca wskaźnik do listy o podanym indeksie w tablicy mieszającej.
 * @param indeks Indeks listy do pobrania.
 * @return Wskaźnik do listy lub nullptr, jeśli indeks jest większy od rozmiaru tablicy.
 */
    LinkedList *getObj(unsigned int indeks)
    {
        if (indeks >= maxSize)
        {
            std::cerr << "\n'getObj(" << indeks << ") indeks wiekszy od tablicy!!\n";
            return nullptr;
        }
        return &array[indeks];
    }
/**
 * Usuwa węzeł o podanym kluczu z tablicy mieszającej.
 * @param key Klucz węzła do usunięcia.
 * @return True, jeśli usunięto węzeł; False, jeśli węzeł nie został znaleziony.
 */
    bool remove(const std::string &key) {
        unsigned int index = hash(key, maxSize);
        LinkedList &listAtIndex = array[index];
        if (listAtIndex.remove(key)) {
            currSize--;
            return true;
        }
        return false;
    }
/**
 * Usuwa wszystkie węzły z tablicy mieszającej.
 */
    bool clear() {
        for (unsigned int i = 0; i < maxSize; ++i) {
            LinkedList *list = getObj(i);
            if (list != nullptr) {
                if (list->first != nullptr) {
                    list->clear();
                }
            }
        }
        currSize = 0;
        return true;
    }
/**
 * Generuje napisową reprezentację tablicy mieszającej.
 * @param count Liczba wpisów do wygenerowania w napisie.
 * @return Napisowa reprezentacja tablicy mieszającej.
 */
    std::string toString(unsigned int count) {
        std::string result;
        result += "\n====================================================================================\nCurrent size = " + std::to_string(currSize);
        result += "\nMax Size = " + std::to_string(maxSize);
        result += "\nProcent Max Uzupelnienia = " + std::to_string(maxFillingPrecentage * 100) +" %";
        result += "\nWspolczynnik wzrosku Tablicy Dynamicznej = " +std::to_string(growth * 100) +" %";
        result += "\nObecny Procent Zapelnienia = " +std::to_string( static_cast<float>(currSize) / static_cast<float>(maxSize) * 100  ) +" %";
        result += "\n\nPierwsze " + std::to_string(count) + " wpisow to tablicy mieszajacej\n\n";

        for (int i = 0; i < maxSize && count > 0; ++i) {
            if (array[i].first) {
                result += "\n====================================================================================\n";
                result += "Index: " + std::to_string(i) + "   ";
                result += array[i].toString(array[i].first) ;
                --count;
            }
        }

        return result;
    }










/**
 * Dodaje nowy węzeł do tablicy mieszającej.
 * Jeśli procent zapełnienia tablicy przekracza określoną wartość,
 * wykonuje procedurę rehashowania.
 * @param key Klucz nowego węzła.
 * @param value Wartość nowego węzła.
 */
    void addObj( const std::string &key, const V &value)
    {
        if ( static_cast<double>(currSize) >=  ( static_cast<double>(maxSize) * maxFillingPrecentage ))
        {
            rehash();
        }
        unsigned int index = hash(key,maxSize);
        LinkedList &listAtIndex = array[index];
        listAtIndex.push_back(key, value);
        currSize++;
    }
protected:
/**
 * Przeskalowuje tablicę mieszającą, zwiększając jej rozmiar o określony współczynnik wzrostu.
 */
    void rehash() {
        unsigned int sizeNewArray = maxSize * growth;
        LinkedList* newArray = new LinkedList[sizeNewArray];

        for (unsigned int i = 0; i < maxSize; ++i) {
            do {
                if (array[i].first != nullptr) {
                    std::string key = array[i].first->key;
                    V value = array[i].first->value;
                    unsigned int newIndex = hash(key,sizeNewArray);
                    newArray[newIndex].push_back(key, value);
                    array[i].first = array[i].first->next;
                }
            } while (array[i].first != nullptr);
        }

        delete[] array;
        array = newArray;
        maxSize = sizeNewArray;
    }
};




int main()
{

    const unsigned int sizeKey = 12;
    Dynamic_Array<O> * tablica = new Dynamic_Array<O>();
    for(int i = 0 ; i < 1000000 ; i ++)
    {
        O* obj = new O();
        std::string key = generateKey(sizeKey);
        tablica->addObj(key,*obj);
    }
    std::cout << tablica->toString(200);


    return 0;



}

