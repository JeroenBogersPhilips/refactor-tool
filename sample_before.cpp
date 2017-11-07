struct X_aa
{
   X_aa(int a, int b) {}

   ~X_aa() {}

   X_aa(const X_aa&);

   X_aa(X_aa&&);

   static void print_underscores() {}
};

void f2()
{

}

template <typename T>
void function_with_underscores(const T&)
{
   f2();
   X_aa::print_underscores();
}

template <typename T>
class X;

template <typename T>
class X<T*>
{
public:
    void some_method_with_underscores()
    {
    	some_method_with_underscores_impl();
    }

	void some_method_with_underscores_impl()
	{
	}
};

template <>
class X<int>
{
public:
	void some_other_method();
};

int main()
{
    X<int*> x;
    x.some_method_with_underscores();

	return 0;
}
