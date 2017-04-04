#ifndef TODO_SHORTCUTS_HH
#define TODO_SHORTCUTS_HH

#include <vector>

namespace todo {

  class shortcut_handler {
    public:
      typedef struct {
        ///< list of input characters for shortcut
        const int*    list;

        ///< size of the shortcut list
        size_t const  size;

        ///< current index in the list
        size_t        idx;

        ///< function to exectute when shortcut is triggered
        void (*execute)(int input);
      } shortcut;

      ///< define shortcuts type
      typedef std::vector<shortcut>                 shortcuts_t;
      typedef typename shortcuts_t::const_iterator  const_iterator;

      ///< define special input characters
      static int const TERMINPUT  = 0x00;
      static int const FWDINPUT   = 0xff;

    private:
      shortcut_handler(shortcuts_t && shcs);
      virtual ~shortcut_handler();

      shortcut_handler() = delete;
      shortcut_handler(shortcut_handler const&) = default;
      shortcut_handler(shortcut_handler &&) = default;

      ///< singleton instance
      static shortcut_handler m_instance;

    public:
      static shortcut_handler& get();

    public:

      inline const_iterator begin() const { return m_shortcuts.begin(); }
      inline const_iterator end() const { return m_shortcuts.end(); }

      void update(int input);

    private:
      shortcuts_t m_shortcuts;
  };

}; // namespace todo

#endif // #ifndef TODO_SHORTCUTS_HH
