class Vector 
{
public:
    float* data = nullptr;
    size_t capacity = 0;
    size_t size = 0;

    Vector() = default;

    Vector(Vector&& rhs) 
        : data(rhs.data),       
          capacity(rhs.capacity), 
          size(rhs.size)
    {
        rhs.data = nullptr;
        rhs.capacity = 0;
        rhs.size = 0;
    }

    ~Vector() { delete[] data; }
};
