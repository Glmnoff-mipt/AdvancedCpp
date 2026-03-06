    #include <fstream>
    #include <string>
    #include <utility>

    class JsonWriter 
    {
    public:
        explicit JsonWriter(const std::string& filename) : file_stream_(filename) 
        {
            if (file_stream_.is_open()) { file_stream_ << "{\n"; }
        }

        ~JsonWriter() { if (file_stream_.is_open()) { file_stream_ << "\n}\n"; } }

        JsonWriter(const JsonWriter&) =            delete;
        JsonWriter& operator=(const JsonWriter&) = delete;

        JsonWriter(JsonWriter&& other) noexcept 
            : file_stream_(std::move(other.file_stream_)), is_first_(other.is_first_) {}

        JsonWriter& operator=(JsonWriter&& other) noexcept 
        {
            if (this != &other) 
            {
                if (file_stream_.is_open()) { file_stream_ << "\n}\n"; }
                file_stream_ = std::move(other.file_stream_);
                is_first_ = other.is_first_;
            }
            return *this;
        }

        void Add(const std::string& key, const std::string& value) 
        {
            if (!file_stream_.is_open()) return;
            
            if (!is_first_) { file_stream_ << ",\n"; } 
            else { is_first_ = false; }
            
            file_stream_ << "  \"" << key << "\": \"" << value << "\"";
        }

    private:
        std::ofstream file_stream_;
        bool is_first_ = true; 
    };

    class UserProfile  
    {
    public:
        explicit UserProfile (const std::string& filename) : writer_(filename) {}

        UserProfile (UserProfile &&) noexcept =            default;
        UserProfile & operator=(UserProfile &&) noexcept = default;

        UserProfile (const UserProfile &) =            delete;
        UserProfile & operator=(const UserProfile &) = delete;

        void Add(const std::string& key, const std::string& value) 
        {
            writer_.Add(key, value);
        }

    private:
        JsonWriter writer_;
    };

int main() { return 0; }
