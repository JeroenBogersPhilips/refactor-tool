struct X_aa
{
   X_aa(int a, int b) {}

   ~X_aa() {}

   X_aa(const X_aa&);

   X_aa(X_aa&&);

   static void printUnderscores() {}
};

void f2()
{

}

template <typename T>
void functionWithUnderscores(const T&)
{
   f2();
   X_aa::printUnderscores();
}

template <typename T>
class X;

template <typename T>
class X<T*>
{
public:
    void someMethodWithUnderscores()
    {
    	someMethodWithUnderscoresImpl();
    }

	void someMethodWithUnderscoresImpl()
	{
	}
};

template <>
class X<int>
{
public:
	void someOtherMethod();
};

int main()
{
    X<int*> x;
    x.someMethodWithUnderscores();

	return 0;
}
