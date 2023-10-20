#ifndef SINGLETON_H_HL_MAI
#define SINGLETON_H_HL_MAI

template <typename T>
class Singleton {
  public:
    static T& GetInstance() {
      static T instance;
      return instance;
    }
  protected:
    Singleton() = default;
    ~Singleton() = default;
  private:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};

#endif
