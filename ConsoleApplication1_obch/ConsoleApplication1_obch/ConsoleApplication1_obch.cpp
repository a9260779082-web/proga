#include "pch.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;

public interface class IPrintable {void Print();};

public ref class Shape abstract {
public:
    virtual double Area() abstract;
    virtual double Perimeter() abstract;
    virtual void Print() { Console::WriteLine("{0}", ToString()); }
    virtual String^ ToString() override {
        return String::Format("Фигура: Площадь={0:F2}, Периметр={1:F2}", Area(), Perimeter());
    }

    virtual String^ Serialize() abstract;
};

public ref class Circle sealed : public Shape, public IPrintable {
    double r;
public:
    Circle(double r) {
        if (r <= 0) throw gcnew ArgumentOutOfRangeException("r");
        this->r = r;
    }
    virtual double Area() override { return Math::PI * r * r; }
    virtual double Perimeter() override { return 2 * Math::PI * r; }
    virtual String^ ToString() override {
        return String::Format("Круг(радиус={0:F2}) Площадь={1:F2} Периметр={2:F2}", r, Area(), Perimeter());
    }
    virtual void Print() override { Shape::Print(); }

    virtual String^ Serialize() override {
        return String::Format("Circle;{0:F2}", r);
    }

    static Circle^ Deserialize(String^ line) {
        array<String^>^ parts = line->Split(';');
        if (parts->Length != 2) throw gcnew FormatException("Неверный формат круга");
        double radius = Double::Parse(parts[1]);
        return gcnew Circle(radius);
    }
};

public ref class Rectangle sealed : public Shape, public IPrintable {
    double w, h;
public:
    Rectangle(double w, double h) {
        if (w <= 0 || h <= 0) throw gcnew ArgumentOutOfRangeException();
        this->w = w; this->h = h;
    }
    virtual double Area() override { return w * h; }
    virtual double Perimeter() override { return 2 * (w + h); }
    virtual String^ ToString() override {
        return String::Format("Прямоугольник({0:F2}x{1:F2}) Площадь={2:F2} Периметр={3:F2}", w, h, Area(), Perimeter());
    }
    virtual void Print() override { Shape::Print(); }

    virtual String^ Serialize() override {
        return String::Format("Rectangle;{0:F2};{1:F2}", w, h);
    }

    static Rectangle^ Deserialize(String^ line) {
        array<String^>^ parts = line->Split(';');
        if (parts->Length != 3) throw gcnew FormatException("Неверный формат прямоугольника");
        double width = Double::Parse(parts[1]);
        double height = Double::Parse(parts[2]);
        return gcnew Rectangle(width, height);
    }
};

public ref class Triangle sealed : public Shape, public IPrintable {
    double a, b, c;
public:
    Triangle(double a, double b, double c) {
        if (a <= 0 || b <= 0 || c <= 0) throw gcnew ArgumentOutOfRangeException();
        if (a + b <= c || a + c <= b || b + c <= a)
            throw gcnew ArgumentException("Нарушено неравенство треугольника");
        this->a = a; this->b = b; this->c = c;
    }
    virtual double Perimeter() override { return a + b + c; }
    virtual double Area() override {
        double p = Perimeter() / 2.0;
        return Math::Sqrt(p * (p - a) * (p - b) * (p - c));
    }
    virtual String^ ToString() override {
        return String::Format("Треугольник({0},{1},{2}) Площадь={3:F2} Периметр={4:F2}", a, b, c, Area(), Perimeter());
    }
    virtual void Print() override { Shape::Print(); }

    virtual String^ Serialize() override {
        return String::Format("Triangle;{0:F2};{1:F2};{2:F2}", a, b, c);
    }

    static Triangle^ Deserialize(String^ line) {
        array<String^>^ parts = line->Split(';');
        if (parts->Length != 4) throw gcnew FormatException("Неверный формат треугольника");
        double side1 = Double::Parse(parts[1]);
        double side2 = Double::Parse(parts[2]);
        double side3 = Double::Parse(parts[3]);
        return gcnew Triangle(side1, side2, side3);
    }
};

int main() {
    Console::Title = "Коллекция фигур";

    try {
        List<Shape^>^ shapes = gcnew List<Shape^>();

        shapes->Add(gcnew Circle(5.0));
        shapes->Add(gcnew Rectangle(4.0, 6.0));
        shapes->Add(gcnew Triangle(3.0, 4.0, 5.0));
        shapes->Add(gcnew Triangle(6.0, 8.0, 10.0));
        shapes->Add(gcnew Circle(2.5));
        shapes->Add(gcnew Rectangle(3.0, 3.0));
 

        Console::WriteLine("\n=== ИНФОРМАЦИЯ О ВСЕХ ФИГУРАХ ===");
        int count = 1;
        for each (Shape ^ shape in shapes) {
            Console::Write("{0}. ", count++);
            shape->Print();
        }


        double totalArea = 0.0;
        double totalPerimeter = 0.0;
        for each (Shape ^ shape in shapes) {
            totalArea += shape->Area();
            totalPerimeter += shape->Perimeter();
        }

        Console::WriteLine("\n=== СВОДНАЯ СТАТИСТИКА ===");
        Console::WriteLine("Общее количество фигур: {0}", shapes->Count);
        Console::WriteLine("Суммарная площадь всех фигур: {0:F2}", totalArea);
        Console::WriteLine("Суммарный периметр всех фигур: {0:F2}", totalPerimeter);

        Console::WriteLine("\n=== АНАЛИЗ ТРЕУГОЛЬНИКОВ ===");
        int triangleCount = 0;
        double totalTriangleArea = 0.0;
        double totalTrianglePerimeter = 0.0;

        for each (Shape ^ shape in shapes) {
            Triangle^ triangle = dynamic_cast<Triangle^>(shape);
            if (triangle != nullptr) {
                triangleCount++;
                totalTriangleArea += triangle->Area();
                totalTrianglePerimeter += triangle->Perimeter();
                Console::WriteLine(triangle->ToString());
            }
        }

        if (triangleCount > 0) {
            Console::WriteLine("\nСтатистика по треугольникам:");
            Console::WriteLine("Количество треугольников: {0}", triangleCount);
            Console::WriteLine("Суммарная площадь треугольников: {0:F2}", totalTriangleArea);
            Console::WriteLine("Суммарный периметр треугольников: {0:F2}", totalTrianglePerimeter);
        }
        else {
            Console::WriteLine("Треугольники не найдены в коллекции");
        }

        String^ filename = "figur.txt";
        try {
            StreamWriter^ writer = gcnew StreamWriter(filename);
            for each (Shape ^ shape in shapes) {
                writer->WriteLine(shape->Serialize());
            }
            writer->Close();
            Console::WriteLine("Коллекция успешно сохранена в файл: {0}", filename);
        }
        catch (Exception^ e) {
            Console::WriteLine("Ошибка при сохранении файла: {0}", e->Message);
        }


        Console::WriteLine("\n=== ЗАГРУЗКА ИЗ ФАЙЛА ===");
        try {
            if (!File::Exists(filename)) {
                Console::WriteLine("Файл не найден: {0}", filename);
            }
            else {
                List<Shape^>^ loadedShapes = gcnew List<Shape^>();
                StreamReader^ reader = gcnew StreamReader(filename);
                String^ line;
                int lineNumber = 0;
                int successCount = 0;

                while ((line = reader->ReadLine()) != nullptr) {
                    lineNumber++;
                    line = line->Trim();

                    if (String::IsNullOrEmpty(line)) continue;

                    try {
                        Shape^ shape = nullptr;
                        array<String^>^ parts = line->Split(';');

                        if (parts[0] == "Circle") {
                            shape = Circle::Deserialize(line);
                        }
                        else if (parts[0] == "Rectangle") {
                            shape = Rectangle::Deserialize(line);
                        }
                        else if (parts[0] == "Triangle") {
                            shape = Triangle::Deserialize(line);
                        }
                        else {
                            throw gcnew FormatException("Неизвестный тип фигуры: " + parts[0]);
                        }

                        if (shape != nullptr) {
                            loadedShapes->Add(shape);
                            successCount++;
                            Console::WriteLine("Строка {0}: {1}", lineNumber, shape->ToString());
                        }
                    }
                    catch (Exception^ e) {
                        Console::WriteLine("Строка {0}: ОШИБКА - {1}", lineNumber, e->Message);
                    }
                }
                reader->Close();

            }
        }
        catch (Exception^ e) {
            Console::WriteLine("Ошибка при чтении файла: {0}", e->Message);
        }

    }
    catch (Exception^ e) {
        Console::WriteLine("Критическая ошибка: {0}", e->Message);
    }

    Console::WriteLine("\nНажмите любую клавишу для выхода...");
    Console::ReadKey();
    return 0;
}
