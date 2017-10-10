/*******************************************************************************************************************************
	Programmer: Ludvik Jerabek
	Date: 02\08\2016

    Defined Classes: state_machine
	Purpose: Provide a state machine template class

    Updates:
        02\08\2016 - Initial Release

*******************************************************************************************************************************/
#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <iostream>
#include <set>
#include <functional>
#include <iomanip>

template <typename state_id_type,typename event_id_type>
class state_machine {
    private:
        struct _state_comparer;
    public:
        typedef state_machine<state_id_type,event_id_type>* state_machine_ptr;
        // State Class Definition
        class state {
            friend state_machine;
            typedef state* state_ptr;
            struct _event_comparer;
        public:
            // Transition Class Definition
            class event {
                friend state;
                typedef event* event_ptr;
                // Public Member Functions
                public:
                    // Returns the current event id
                    event_id_type get_id() const;
                    // Returns the state transition associated with the event
                    state_ptr get_state() const;
                    // Return true if action has been defined
                    bool has_action() const;
                    // Bind action to this event
                    void bind_action(std::function<void(event_ptr,state_ptr,state_ptr)> action);
                // Private Member Functions
                private:
                    event(event_id_type event_id, state_ptr target_state);
                    ~event();
                // Private Types
                private:
                    event_id_type _id;
                    state_ptr _state;
                    std::function<void(event_ptr,state_ptr,state_ptr)> _action;
            };
        typedef event* event_ptr;
        // Private Member Type Definitions
        private:
            struct _event_comparer {
                bool operator()(const event_ptr lhs, const event_ptr rhs) const {
                    return rhs->_id < lhs->_id;
                }
            };
            typedef std::set<event_ptr,_event_comparer> event_set;
        // Public Member Functions
        public:
            state_id_type get_id() const;
            bool is_accepted() const;
            bool is_terminal() const;
            // Creates a null next event
            event_ptr add_event(event_id_type event_id);
            event_ptr add_event(event_id_type event_id, state_id_type next_state, bool accepted = false);
            event_ptr get_event(event_id_type event_id) const;
            bool has_entry_action() const;
            bool has_exit_action() const;
            state_ptr bind_entry_action(std::function<void(state_ptr/*current_state*/)> entry_action);
            state_ptr bind_exit_action(std::function<void(state_ptr/*next_state*/)> exit_action);
            const event_set& get_events() const;
        // Private Member Function
        private:
            state(state_machine* parent, state_id_type state_id, bool accepted);
            ~state();
            void _do_action(event_ptr event);
            typename event_set::const_iterator _find_event(event_id_type event_id) const;
            typename event_set::iterator _find_event(event_id_type event_id);
        // Private Types
        private:
            state_machine* _parent; // Pointer to parent
            state_id_type _id;
            bool _accepted;
            event_set _events;
            std::function<void(state_ptr/*current_state*/)> _entry_action;
            std::function<void(state_ptr/*next_state*/)> _exit_action;
    };
    typedef state* state_ptr;
    // Private Member Type Definitions
    private:
        struct _state_comparer {
            bool operator()(const state_ptr lhs, const state_ptr rhs) const {
            return rhs->_id < lhs->_id;
            }
        };
        typedef std::set<state_ptr,_state_comparer> state_set;
    // Public Member Functions
    public:
        state_machine();
        ~state_machine();
        state_ptr add_state(state_id_type state_id, bool accepted = false);
        state_ptr get_state(state_id_type state_id) const;
        state_ptr get_current_state() const;
        state_ptr get_initial_state() const;
        state_ptr next_state(event_id_type event_id) throw(std::runtime_error);
        state_ptr reset();

        const state_set& get_states() const;
        void dump_state_table();
    // Private Member Functions
    private:
        typename state_set::const_iterator _find_state(state_id_type state_id) const;
        typename state_set::iterator _find_state(state_id_type state_id);
    // Private Types
    private:
        state_set _states;
        state_ptr _initial;
        state_ptr _current;
    public:
};

/*************************************************************************
*
*   Event Member Functionleftleft Definitions
*
*************************************************************************/

template <typename state_id_type,typename event_id_type>
event_id_type state_machine<state_id_type,event_id_type>::state::event::get_id() const {
    return _id;
}

template <typename state_id_type,typename event_id_type>
typename  state_machine<state_id_type,event_id_type>::state::state_ptr
state_machine<state_id_type,event_id_type>::state::event::get_state() const {
    return _state;
}

template <typename state_id_type,typename event_id_type>
bool state_machine<state_id_type,event_id_type>::state::event::has_action() const {
    return (bool)_action;
}


template <typename state_id_type,typename event_id_type>
void state_machine<state_id_type,event_id_type>::state::event::bind_action(std::function<void(event_ptr,state_ptr,state_ptr)> action){
    _action = action;
}


template <typename state_id_type,typename event_id_type>
state_machine<state_id_type,event_id_type>::state::event::event(event_id_type event_id, state_ptr target_state)
: _id(event_id) , _state(target_state) { }

template <typename state_id_type,typename event_id_type>
state_machine<state_id_type,event_id_type>::state::event::~event() {
}



/*************************************************************************
*
*   State Member Function Definitions
*
*************************************************************************/

template <typename state_id_type,typename event_id_type>
state_id_type state_machine<state_id_type,event_id_type>::state::get_id() const {
    return _id;
}

template <typename state_id_type,typename event_id_type>
bool state_machine<state_id_type,event_id_type>::state::is_accepted() const {
    return _accepted;
}

template <typename state_id_type,typename event_id_type>
bool state_machine<state_id_type,event_id_type>::state::is_terminal() const {
    return _events.size() == 0;
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state::event_ptr
state_machine<state_id_type,event_id_type>::state::add_event(event_id_type event_id) {
    typename event_set::const_iterator itr = _find_event( event_id );
    if( itr == _events.end() )
    {
        // No state change...
        event_ptr t = new event(event_id, this);
        _events.insert(t);
        return t;
    }
    else
        throw std::runtime_error("Attemted state transiton mangling");
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state::event_ptr
state_machine<state_id_type,event_id_type>::state::add_event(event_id_type event_id, state_id_type next_state, bool accepted) {
    typename event_set::const_iterator itr = _find_event( event_id );
    // Get the state or create it
    state_ptr s = _parent->add_state(next_state,accepted);
    if( itr == _events.end() )
    {
        event_ptr t = new event(event_id, s);
        _events.insert(t);
        return t;
    }
    else
        throw std::runtime_error("Attemted state transiton mangling");
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state::event_ptr
state_machine<state_id_type,event_id_type>::state::get_event(event_id_type event_id) const {
    typename event_set::const_iterator itr = _find_event( event_id );
    if( itr != _events.end() )
        return *itr;
    return nullptr;
}

template <typename state_id_type,typename event_id_type>
void state_machine<state_id_type,event_id_type>::state::_do_action(event_ptr event) {
    event->_action(event,this,event->get_state());
}

template <typename state_id_type,typename event_id_type>
state_machine<state_id_type,event_id_type>::state::state(state_machine* parent, state_id_type state_id, bool accepted)
: _parent(parent) , _id(state_id), _accepted(accepted) , _events() { }

template <typename state_id_type,typename event_id_type>
state_machine<state_id_type,event_id_type>::state::~state() {
    for( auto t : _events )
        delete t;
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state::event_set::const_iterator
state_machine<state_id_type,event_id_type>::state::_find_event(event_id_type event_id) const {
    event bogus(event_id,nullptr);
    return _events.find( &bogus );
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state::event_set::iterator
state_machine<state_id_type,event_id_type>::state::_find_event(event_id_type event_id) {
    event bogus(event_id,nullptr);
    return _events.find( &bogus );
}

template <typename state_id_type,typename event_id_type>
bool state_machine<state_id_type,event_id_type>::state::has_entry_action() const {
    return (bool)_entry_action;
}

template <typename state_id_type,typename event_id_type>
bool state_machine<state_id_type,event_id_type>::state::has_exit_action() const {
    return (bool)_exit_action;
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state::state_ptr
state_machine<state_id_type,event_id_type>::state::bind_entry_action(std::function<void(state_ptr/*current_state*/)> entry_action) {
    _entry_action = entry_action;
    return this;
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state::state_ptr
state_machine<state_id_type,event_id_type>::state::bind_exit_action(std::function<void(state_ptr/*next_state*/)> exit_action) {
    _exit_action = exit_action;
    return this;
}

template <typename state_id_type,typename event_id_type>
const typename state_machine<state_id_type,event_id_type>::state::event_set &
state_machine<state_id_type,event_id_type>::state::get_events() const {
    return _events;
}

/*************************************************************************
*
*   State Machine Member Function Definitions
*
*************************************************************************/

// State machine construstor
template <typename state_id_type,typename event_id_type>
state_machine<state_id_type,event_id_type>::state_machine()
: _states(), _initial(nullptr) , _current(nullptr) {}

// State machine destructor
template <typename state_id_type,typename event_id_type>
state_machine<state_id_type,event_id_type>::~state_machine() {
    for( auto s : _states )
        delete s;
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state_ptr
state_machine<state_id_type,event_id_type>::add_state(state_id_type state_id, bool accepted ) {
    typename state_set::const_iterator itr = _find_state( state_id );
    if( itr == _states.end() )
    {
        state_ptr s = new state(this,state_id,accepted);
        // First state added is set to initial state by default
        if( _states.size() == 0 )
        {
            _initial = s;
            _current = _initial;
        }
        _states.insert(s);
        return s;
    }
    else
        return *itr;
}


template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state_ptr
state_machine<state_id_type,event_id_type>::get_state(state_id_type state_id) const {
    typename state_set::const_iterator itr = _find_state( state_id );
    if( itr != _states.end() )
        return *itr;
    return nullptr;
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state_ptr
state_machine<state_id_type,event_id_type>::get_current_state() const {
return _current;
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state_ptr
state_machine<state_id_type,event_id_type>::get_initial_state() const {
return _initial;
}


template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state_ptr
state_machine<state_id_type,event_id_type>::next_state(event_id_type event_id) throw(std::runtime_error) {
    // From the current state get the event by id
    typename state::event_ptr event = _current->get_event(event_id);
    if( event == nullptr ) throw std::runtime_error("No event for this event");

    state_ptr next_state = event->get_state();
    if( event->has_action() )
        _current->_do_action(event);

    if( _current != next_state )
    {
        if( _current->has_exit_action() )
            _current->_exit_action( next_state );
        if( next_state->has_entry_action() )
            next_state->_entry_action( _current );
        _current = next_state;
    }
    return _current;
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state_ptr
state_machine<state_id_type,event_id_type>::reset() {
    _current = _initial;
    return _current;
}

template <typename state_id_type,typename event_id_type>
const typename state_machine<state_id_type,event_id_type>::state_set&
state_machine<state_id_type,event_id_type>::get_states() const{
    return _states;
}

template <typename state_id_type,typename event_id_type>
void state_machine<state_id_type,event_id_type>::dump_state_table() {
    std::cout << std::left << std::setfill(' ')
              << std::setw(16) << "STATE"
              << std::setw(16) << "ACCEPTED"
              << std::setw(16) << "TERMINAL"
              << std::setw(16) << "EVENT"
              << std::setw(16) << "ACTON"
              << std::setw(16) << "NEXT_STATE" << std::endl;
    for( auto state : _states ) {
        if( state->get_events().size() == 0 )
            std::cout << std::left << std::setfill(' ')
                      << std::setw(16) << state->get_id()
                      << std::setw(16) << std::boolalpha << state->is_accepted()
                      << std::setw(16) << std::boolalpha << state->is_terminal()
                      << std::setw(16) << "---"
                      << std::setw(16) << "---"
                      << std::setw(16) << "---" << std::endl;
        for( auto event : state->get_events() )
        {
            std::cout << std::left << std::setfill(' ')
                      << std::setw(16) << state->get_id()
                      << std::setw(16) << std::boolalpha << state->is_accepted()
                      << std::setw(16) << std::boolalpha << state->is_terminal()
                      << std::setw(16) << event->get_id()
                      << std::setw(16) << std::boolalpha << event->has_action()
                      << std::setw(16) << ((event->get_state() == nullptr ) ? "---" : event->get_state()->get_id() ) << std::endl;
        }
    }
    std::cout << std::setfill('-') << std::setw(80) << "" << std::endl;
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state_set::const_iterator
state_machine<state_id_type,event_id_type>::_find_state(state_id_type state_id) const {
    state bogus(nullptr,state_id,false);
    return _states.find( &bogus );
}

template <typename state_id_type,typename event_id_type>
typename state_machine<state_id_type,event_id_type>::state_set::iterator
state_machine<state_id_type,event_id_type>::_find_state(state_id_type state_id) {
    state bogus(nullptr,state_id,false);
    return _states.find( &bogus );
}

#endif //STATE_MACHINE_H_
