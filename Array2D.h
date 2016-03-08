#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <stdexcept>

template<class T>
class Array2D{
  private:
    int w, h;
    T* data;
    
  public:
    Array2D() = delete;
    Array2D(Array2D<T>& rhs);
    Array2D(Array2D<T>&& rhs);
    Array2D(int w, int h, T v);
    
    ~Array2D();
    
    const T& operator()(int x, int y) const;
    T& operator()(int x, int y);

    const T* raw() const;
    T*  raw();
    
    bool inBounds(int x, int y) const;
    int width() const;
    int height() const;
};

template<class T>
Array2D<T>::Array2D(Array2D<T>& rhs)
:w(rhs.w)
,h(rhs.h)
,data(new T[w * h])
{
    for(int i = 0; i < w * h; ++i){
      data[i] = rhs.data[i];
    }
}

template<class T>
Array2D<T>::Array2D(Array2D<T>&& rhs)
:w(rhs.w)
,h(rhs.h)
,data(rhs.data)
{
    rhs.data = nullptr;
}

template<class T>
Array2D<T>::Array2D(int w, int h, T v)
:w(w)
,h(h)
,data(new T[w * h])
{
    for(int i = 0; i < w * h; ++i){
      data[i] = v;
    }
}

template<class T>
Array2D<T>::~Array2D(){
  if(data != nullptr)
    delete [] data;
}

template<class T>
const T& Array2D<T>::operator()(int x, int y) const{
  #ifndef STX_NO_ERROR_CHECKING
  if(!inBounds(x, y))
    throw std::out_of_range("");
  #endif
    
  return data[x + y * w];
}

template<class T>
T& Array2D<T>::operator()(int x, int y){
  #ifndef STX_NO_ERROR_CHECKING
  if(!inBounds(x, y))
    throw std::out_of_range("");
  #endif
  
  return data[x + y * w];
}

template<class T>
const T* Array2D<T>::raw() const{
  return data;
}

template<class T>
T* Array2D<T>::raw(){
  return data;
}

template<class T>
bool Array2D<T>::inBounds(int x, int y) const{
  return (x >= 0 && x < w && y >= 0 && y < h);
}

template<class T>
int Array2D<T>::width() const{
  return w;
}

template<class T>
int Array2D<T>::height() const{
  return h;
}

#endif // ARRAY2D_H
