#include <iostream>

template<typename T>
class Stack{
    private:
        int capacity = 0;
        std::size_t size = 10; //default as 10
        T *array; //array
        
        bool is_full(){
            if (capacity == size)
            {
                return true;
            }

            return false;
        }

        bool is_empty(){
            if (capacity == 0)
            {
                return true;
            }
            return false;
        }

    public:
        explicit Stack(){
            array = new T [size];
        }

        /**Returns current size of stack */
        int get_size(){
            return size;
        }

        /**resize stack*/
        void resize(int newSize){
            T* newArray = new T[newSize];
            for (int i = 0; i < capacity; ++i) {
                newArray[i] = array[i];
            }
            delete[] array;
            array = newArray;
            size = newSize;
        }
        
        /**push*/
        void push(T item){
            if(is_full() == true){
                resize(size * 2);
                push(item);
            }
            array[capacity] = item;
            capacity++;
        }

        /** Pop */
        auto pop(){
            if (is_empty() == true)
            {
                return NULL;
            }
            capacity--;
            return array[capacity];
        }

        ~Stack(){
            delete[] array;
        }
};

int main(){
    Stack<int> st;

    std::cout << "SIZE: " << st.get_size() << "\n";
    for (int i = 0; i < 14; i++)
    {
        st.push(i);
    }
    
    for (int j = 0; j < 15; j++)
    {
        std::cout << "POP!: " << st.pop() << std::endl;
    }
    
    std::cout << st.get_size() << std::endl;
    st.resize(30);
    std::cout << st.get_size() << std::endl;
    
    return 0;
}