/*
 
 Visitor & visitable base classes to make creating visitors a lot more convenient.
 http://stackoverflow.com/questions/7876168/using-the-visitor-pattern-with-template-derived-classes
 
 Example of use:
 
 class Visitable1 : public BaseVisitable<Visitable1>
 {
 };

 class Visitable2 : public BaseVisitable<Visitable2>
 {
 };

 class VisitorDerived : public BaseVisitor,
 public Visitor<Visitable1>,
 public Visitor<Visitable2>
 {
 public:
     void visit(Visitable1 & c)
     {
         std::cout << __PRETTY_FUNCTION__ << std::endl;
     }
     void visit(Visitable2 & c)
     {
         std::cout << __PRETTY_FUNCTION__ << std::endl;
     }
 };

 */

#pragma once
#include <iostream>

class BaseVisitor
{
public:
    virtual ~BaseVisitor() {};
};

template <class T>
class Visitor
{
public:
    virtual void visit(T &) = 0;
};

template <class Visitable>
class BaseVisitable
{
public:
    template <typename T>
    void accept(T & visitor)
    {
        visitor.visit(static_cast<Visitable &>(*this));
    }
};

/** example of use */
template <typename Annotation>
class Visitable1 : public BaseVisitable<Visitable1<Annotation>>
{
    Annotation annotation;
};

class Visitable2 : public BaseVisitable<Visitable2>
{
};

class VisitorDerived : public BaseVisitor,
public Visitor<Visitable1<int>>,
public Visitor<Visitable2>
{
public:
    void visit(Visitable1<int>& c)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    void visit(Visitable2 & c)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};
