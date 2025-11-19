#include "pch.h"

using namespace System;
using namespace System::Collections::Generic;


public ref class Product abstract
{
public:
    virtual property String^ Name;
    virtual property double Price;
    virtual property double DiscountPercent;
    virtual property String^ Category;

    Product(String^ name, double price, double discountPercent, String^ category)
    {
        if (price < 0) throw gcnew ArgumentOutOfRangeException("price");
        if (discountPercent < 0 || discountPercent > 100)
            throw gcnew ArgumentOutOfRangeException("discountPercent");

        Name = name;
        Price = price;
        DiscountPercent = discountPercent;
        Category = category;
    }

 
    virtual double GetDiscountedPrice()
    {
        return Price * (1 - DiscountPercent / 100.0);
    }

    virtual String^ ToString() override
    {
        return String::Format("{0} | {1} | {2}% | {3}",
            Name, Price, DiscountPercent, GetDiscountedPrice());
    }
};


public ref class Food : public Product
{
public:
    Food(String^ name, double price, double discountPercent)
        : Product(name, price, discountPercent, "Продукты")
    {
    }
};


public ref class Electronics : public Product
{
public:
    Electronics(String^ name, double price, double discountPercent)
        : Product(name, price, discountPercent, "Электроника")
    {
    }
};


public ref class Clothing : public Product
{
public:
    Clothing(String^ name, double price, double discountPercent)
        : Product(name, price, discountPercent, "Одежда")
    {
    }
};

public ref class ProductItem
{
public:
    property Product^ ItemProduct;
    property int Kolichestvo;

    ProductItem(Product^ product, int kolichestvo)
    {
        ItemProduct = product;
        Kolichestvo = kolichestvo;
    }

    double GetTotalPrice()
    {
        return ItemProduct->GetDiscountedPrice() * Kolichestvo;
    }
};


int main(array<System::String^>^ args)
{
    List<ProductItem^>^ cart = gcnew List<ProductItem^>();

    Console::WriteLine("=== ДОБАВЛЕНИЕ ТОВАРОВ В КОРЗИНУ ===");
    Console::WriteLine("Доступные категории: 1-Продукты, 2-Электроника, 3-Одежда");
    Console::WriteLine("Для завершения ввода введите 'exit' вместо названия товара\n");

 
    while (true)
    {
        try
        {
            Console::Write("Название товара: ");
            String^ name = Console::ReadLine();
            if (name->ToLower() == "exit") break;
            if (String::IsNullOrEmpty(name)) continue;

            Console::Write("Категория (1-3): ");
            String^ categoryInput = Console::ReadLine();
            int categoryChoice = Convert::ToInt32(categoryInput);

            Console::Write("Цена: ");
            String^ priceInput = Console::ReadLine();
            double price = Convert::ToDouble(priceInput);

            Console::Write("Скидка (%): ");
            String^ discountInput = Console::ReadLine();
            double discount = Convert::ToDouble(discountInput);

            Console::Write("Количество: ");
            String^ kolichestvoInput = Console::ReadLine();
            int kolichestvo = Convert::ToInt32(kolichestvoInput);

            Product^ product;
            switch (categoryChoice)
            {
            case 1:
                product = gcnew Food(name, price, discount);
                break;
            case 2:
                product = gcnew Electronics(name, price, discount);
                break;
            case 3:
                product = gcnew Clothing(name, price, discount);
                break;
            default:
                throw gcnew ArgumentException("Неверная категория");
            }

            cart->Add(gcnew ProductItem(product, kolichestvo));
            Console::WriteLine("Товар добавлен!\n");
        }
        catch (Exception^ ex)
        {
            Console::WriteLine("Ошибка: {0}\n", ex->Message);
        }
    }


    if (cart->Count == 0)
    {
        Console::WriteLine("Корзина пуста!");
        return 0;
    }


    Console::WriteLine("\n=== ВСЕ ТОВАРЫ ===");
    for each (ProductItem ^ item in cart)
    {
        Console::WriteLine("{0} - {1} шт. - {2} за шт. - {3} всего",
            item->ItemProduct->Name,
            item->Kolichestvo,
            item->ItemProduct->GetDiscountedPrice(),
            item->GetTotalPrice());
    }

    Console::WriteLine("\n" + gcnew String('=', 60));
    Console::WriteLine("ЧЕК");
    Console::WriteLine(gcnew String('=', 60));

    double grandTotal = 0.0;

    Dictionary<String^, List<ProductItem^>^>^ categories = gcnew Dictionary<String^, List<ProductItem^>^>();

    for each (ProductItem ^ item in cart)
    {
        String^ category = item->ItemProduct->Category;
        if (!categories->ContainsKey(category))
        {
            categories[category] = gcnew List<ProductItem^>();
        }
        categories[category]->Add(item);
    }

    List<String^>^ sortedCategories = gcnew List<String^>(categories->Keys);
    sortedCategories->Sort();

    for each (String ^ category in sortedCategories)
    {
        Console::WriteLine("\n{0}:", category);
        Console::WriteLine(gcnew String('-', 60));
        Console::WriteLine("{0,-20} {1,-10} {2,-15} {3,-15}",
            "Название", "Кол-во", "Цена за шт.", "Сумма");
        Console::WriteLine(gcnew String('-', 60));

        double categoryTotal = 0.0;

        List<ProductItem^>^ productsInCategory = categories[category];


        for (int i = 0; i < productsInCategory->Count - 1; i++)
        {
            for (int j = 0; j < productsInCategory->Count - i - 1; j++)
            {
                if (productsInCategory[j]->ItemProduct->GetDiscountedPrice() >
                    productsInCategory[j + 1]->ItemProduct->GetDiscountedPrice())
                {
                    ProductItem^ temp = productsInCategory[j];
                    productsInCategory[j] = productsInCategory[j + 1];
                    productsInCategory[j + 1] = temp;
                }
            }
        }

        for each (ProductItem ^ item in productsInCategory)
        {
            double itemTotal = item->GetTotalPrice();
            categoryTotal += itemTotal;

            Console::WriteLine("{0,-20} {1,-10} {2,-15} {3,-15}",
                item->ItemProduct->Name,
                item->Kolichestvo,
                item->ItemProduct->GetDiscountedPrice(),
                itemTotal);
        }

        Console::WriteLine(gcnew String('-', 60));
        Console::WriteLine("{0,-45} {1,-15}", "Итого по категории:", categoryTotal);
        grandTotal += categoryTotal;
    }

    Console::WriteLine(gcnew String('=', 60));
    Console::WriteLine("{0,-45} {1,-15}", "ОБЩАЯ СУММА:", grandTotal);
    Console::WriteLine(gcnew String('=', 60));

    Console::ReadLine();
    return 0;
}
