#include <iostream>
#include <string>

#include <td-defs.h>
#include <td-widget.h>
#include <td-except.h>
#include <td-update.h>

class test_widget : public todo::widget, public todo::update_if {
  public:
    test_widget(std::string name) : todo::widget(), m_name(name) {
      set_update(new todo::urecall_exception(this));
    }
    virtual ~test_widget() {}
    virtual void callback_handler(int input) {
      std::cout << m_name << " got input: 0x" << std::hex << input
        << std::dec;

      if(CMDK_VALID_START <= input && input <= CMDK_VALID_END)
        std::cout << " = '" << (char)input << "'";

      switch(input) {
        case CMDK_ESCAPE:
          std::cout << " = CMDK_ESCAPE; returning focus" << std::endl;
          return_focus(); // throws exception
          break;
        case 'r':
        case 'R':
          std::cout << "; reprint" << std::endl;
          // R eprint
          update();
          break;
      }

      // this might be not executed! >
      std::cout << std::endl;
    }
    virtual int print(WINDOW * win = NULL) {
      (void)win;
      std::cout << m_name << " " << this << std::endl;
      return 1;
    }
    std::string name() const { return m_name; }
  private:
   std::string m_name;
};

class test_deep_widget : public test_widget {
  public:
    test_deep_widget(std::string name):
      test_widget(name),
      m_test(name+".test") {}
    virtual ~test_deep_widget() {}
    virtual int print(WINDOW * win = NULL) {
      test_widget::print();
      std::cout << "  -> ";
      m_test.print();
      return 2;
    }
    virtual void callback(int input) {
      m_test.callback(input);
    }
  private:
    test_widget m_test;
};

class test_system : public test_widget {
  public:
    test_system():
      test_widget("system"),
      m_test1("test1"),
      m_test2("test2"),
      m_test3("test3")
    {
      update();
    }

    virtual ~test_system() {}

    virtual int print(WINDOW * win = NULL) {
      int lines = test_widget::print();
      std::cout << "  -> ";
      lines += m_test1.print();
      std::cout << "  -> ";
      lines += m_test2.print();
      std::cout << "  -> ";
      lines += m_test3.print();
      return lines;
    }

    virtual void update() {
      int lines = print();
      std::cout << "---- lines: " << lines << " ----" << std::endl;
    }

    virtual void callback_handler(int input) {
      try {
        test_widget::callback_handler(input);
      } catch(todo::exception * except) {
        std::cout << name() << " catched exception" << std::endl;
        except->handle(this);
      }
      switch(input) {
        case '1':
          std::cout << " -> set focus to: ";
          m_test1.print();
          set_focus(&m_test1);
          break;
        case '2':
          std::cout << " -> set focus to: ";
          m_test2.print();
          set_focus(&m_test2);
          break;
        case '3':
          std::cout << " -> set focus to: ";
          m_test3.print();
          set_focus(&m_test3);
          break;
        default:
          std::cout << " -> invalid input, ignored" << std::endl;
          break;
      }
    }
  private:
    test_widget m_test1;
    test_widget m_test2;
    test_deep_widget m_test3;
};

int main() {
  test_system sys;
  const int str[] = { 0xCAFECAFE,
                      '1',
                      0x7E571,
                      CMDK_ESCAPE,
                      0xCAFECAFE,
                      '2',
                      0x7E572,
                      CMDK_ESCAPE,
                      0xCAFECAFE,
                      '3',
                      0x7E573,
                      'r',
                      CMDK_ESCAPE,
                      0xCAFECAFE,
                      CMDK_ESCAPE,
                      0xCAFECAFE };

  const int length = sizeof(str)/sizeof(int);
  for(int l = 0; l < length; l++) {
    sys.callback(str[l]);
  }
}
