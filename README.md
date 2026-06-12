# CWire
This is CWire, toolkit library for C++ reactive programming.

## Installation
### Get files
Choose from options:
- Download the latest release from [here](url) (currently unavailable),
- Reference library via repository url,
- Fork project and compile it yourself

### Add references
Once you get your files, drop library files in your project directory and add references to them.

## Guide
Now let's go through the contents of the library. Though modest, it contains very versatile utils.
It introduces the paradigm of reactive programming to C++ using special function wrappers. All are members of the `cwr` namespace, each in their respective header file.

### Delegates

Delegates are the cornerstone of this library, a generic and polymorphic function containers. First type parameter - `R`, represents the return type, following ones are argument types in their respective order. Example delegate is declared as follows:

```Cpp
#include <delegate>

cwr::delegate<void, const char*> logger;
```
> This delegate stores any functions of signature `void(const char*)`

To make things easier we can alias our delegate type as such:
```Cpp
using loggerDlg = cwr::delegate<void, const char*>;
```

Now let's define some printing functions:
```Cpp
#include <iostream>

struct MyStruct {
  void member_print(const char* str) {
    std::cout << "From member: " << str << std::endl();
  }

  static void static_print(const char* str) {
    std::cout << "From static: " << str << std::endl();
  }
}

void global_print(const char* str) {
  std::cout << "From global: " << str << std::endl();
}
```

We can store any of it in our delegate variable using `of<>()` and `set<>()` function and call it with both `invoke(Args...)` and `operator()(Args...)`;

```Cpp
int main() {
  printerDlg printer;
  printer = printerDlg::of<&global_print>();
  printer.invoke("A"); // From global: A

  printer.set<&MyStruct::global_print>();
  printer("B"); // From static: B

  MyStruct myStruct;
  printer.set<MyStruct, &MyStruct::member_print>(myStruct);
  printer.invoke("C"); // From member: C

  printer.set<[](const char* str){}>
}
```
> It's not hard to notice that member function is assigned a bit differently. This is due to the requirement of the object it's called on. That's why we pass `myStruct` variable as a context.

Because we passed `void` as return type, for obvious reasons we don't get anything in return. We can as well return any type available and, as expected, delegate invocation will change its signature respectively, allowing for assignment like this:

```Cpp
int increment(int v) {
  return v + 1;
}

int main() {
  auto modifier = cwr::delegate<int, int>::of<&increment>();

  int a = 10;
  a = modifier(a);
  std::cout << a << std::endl(); // 11
}
```

Along with delegate class come some useful generic aliases:
- `supplier<T>` - accepts no arguments and returns `T`
- `consumer<Args...>` - accepts number of arguments, returns `void`
- `predicate<Args...>` - accepts number of arguments and returns `bool`
- `mapper<K, V>` - accepts `V` and returns `K`

### Multidelegates

Multidelegates store multiple delegates of same type and invoke the, on call in an ordered manner. They have same signature as delegates, but provide some additional functionality.

To manage its contents we use `add(delegate)` and `remove(delegate)` methods or `+=` and `-=` operators.

```Cpp
multidelegate<int> numGetter;

auto d1 = supplier<int>::of<[](){return 1;}>()
auto d2 = supplier<int>::of<[](){return 2;}>()

numGetter.add(d1);
numGetter += d2;

std::vector<int> results;
numGetter(&results);

// results = {1, 2}
```

### Events

Events are subtypes of multidelegate returning `void` that introduce special restriction. The event can only be called from within the class/struct it's defined in. We can define such event using `EVENT` macro as such:

```Cpp
class MyClass {
  EVENT(MessageSent, class MyClass, std::string);
}
```
> Event requires name of event, parent class and event type paramteres

Such code will produce a field, in this case called `onMessageSent`. This field behaves as multidelegate, except it's onle callable inside `MyClass`. Delegates with respective signature can be still added and removed.

```Cpp
class MyClass {
  EVENT(MessageSent, class MyClass, std::string);

  void sendMessage(std::string msg) {
    onMessageSent.invoke(msg);
  }
}

void onMyClassMessageSent(std::string msg) {
  std::cout << msg << std::endl();
}

int main() {
  using Handler = delegate<void, std::string>;

  MyClass myClass;
  myClass.onMessageSent += Handler::of<&onMyClassMessageSent>();

  myClass.sendMessage("First message"); // No error

  myClass.onMessageSent.invoke("Second message"); // Won't compile!
}
```

### Property

Properties are built-in value wrappers invoking events on value change. A property of int can be created like this:

```Cpp
property<int> value = 0;
```

It contains `onValueChanged` event field which, as explained before, cannot be called, only subscribed to. If we'd want to do so, it would be no different than before:

```Cpp
void onIntValueChanged(int oldValue, int newValue) {
  std::cout << oldValue << " -> " << newValue << std::endl();
}

int main() {
  cwr::property<int> value = 0;

  using Handler = cwr::property<int>::ValueChangedEventHandler
  value.onValueChanged += Handler::of<&onIntValueChanged>();

  value = 1; // 0 -> 1
}
```

If property is marked as `const` it's value can't be changed.

```Cpp
const property<int> value = 0;
value = 1 // Won't compile!
```
