#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP
#include "exceptions.hpp"
#include <iostream>
const int NN = 300; //In {150,200,250,300}, 300 is the best

namespace sjtu {
    template<class T>
    class list {
        struct node {
            T    *value;
            node *pre;
            node *next;
            node(const T &v) {
                value = new T(v);
                pre = nullptr;
                next = nullptr;
            }
            // node() {
            //     value = (T*)malloc(sizeof(T));
            //     pre = nullptr;
            //     next = nullptr;
            // }
            ~node(){
                delete value;
            }
        };
        node * head;
        node * tail;
        size_t size;
    public:
        class iterator{
            list<T> * rt;
            node *pos;
            size_t index;
        public:
            list<T> * getroot() const{
                return rt;
            }

            iterator(){
                rt = nullptr;
                pos = nullptr;
                index = 0;
            }

            iterator(list<T> * _rt,node * _pos, size_t _index){
                rt = _rt;
                pos = _pos;
                index = _index;
            }

            iterator & operator ++(){ //++iterator
                pos = pos->next;
                ++index;
                return *this;
            }

            const iterator operator ++(int){ //iterator++
                node* tmp = pos;
                pos = pos->next;
                return iterator(rt, tmp, index++);
            }

            iterator operator +(size_t u) const{ 
                node* tmp = pos;
                for(size_t i = 0; i < u; ++i) tmp = tmp->next;
                return iterator(rt, tmp, index+u);
            }

            iterator operator -(size_t u) const{
                if(!pos){ //if(*this == rt -> getend())
                    return u ? rt->gettail() - (u-1) : rt->getend();
                }//注意链表的end指向的是空指针!要转换成tail
                node* tmp = pos;
                for(size_t i = 0; i < u; ++i) tmp = tmp->pre;
                return iterator(rt, tmp, index-u);
            }

            iterator& operator +=(size_t u){
                return *this = *this + u;
            }

            iterator& operator -=(size_t u){
                return *this = *this - u;
            }

            iterator operator --(){ //--iterator
                if(!pos){ //if(*this == rt -> getend())
                    return *this = rt->gettail(); 
                }
                pos = pos->pre;
                --index;
                return *this;
            }

            friend void connect(iterator p,iterator q) {
                p.pos->next = q.pos;
                q.pos->pre = p.pos;
            }

            friend void cutoff(iterator p,iterator q){
                p.pos->next = nullptr;
                q.pos->pre = nullptr;
            }
            
            // node* getpos(){
            //     return pos;
            // }

            operator node *(){ 
                return pos;
            }

            void destroy_pos() const{
                delete pos;
            }

            const iterator operator --(int){
                pos = pos->pre;
                node * ret = pos;
                return iterator(rt, ret, index--);
            }

            int operator -(const iterator &it) const{ 
                return index - it.index;                
            }

            T& operator *() const{
                return *(pos->value);
            }

            T * operator ->() const{
                return pos->value;
            }

            bool available() const{ //如果pos不为空指针,则该链表available
                return pos != nullptr;
            }

            bool operator ==(const iterator &it) const{
                return pos == it.pos && index == it.index;
            }

            bool operator !=(const iterator &it) const{
                return pos != it.pos || index != it.index;
            }
        };

        list(){
            head = nullptr;
            tail = nullptr;
            size = 0;
        }

        list(node * const & _head,node * const & _tail,const size_t & _size){
            head = _head;
            tail = _tail;
            size = _size;
        }

        list(const list &other){
            head = nullptr;
            tail = nullptr;
            size = 0;
            for(iterator i = other.getbegin(); i != other.getend(); ++i){
                push_back(*i);
            }
        }

        list & operator=(const list &other){
            if(&other == this) return *this;
            clear_list();
            for(iterator i = other.getbegin(); i != other.getend(); ++i){
                push_back(*i);
            }
            return *this;
        }

        ~list(){
            // for(iterator it = getbegin(); it != getend(); ){
            //     (it++).destroy_pos();
            // }
            while(size) erase(gettail());
        }

        void clear_list(){
            while(size) erase(gettail());
        }

        void push_back(const T &v) {
            insert(getend(), v);
        }

        void push_front(const T &v) {
            insert(getbegin(), v);
        }

        void pop_front(){
            erase(getbegin());
        }

        void pop_back(){
            erase(gettail());
        }

        iterator getbegin() const{ //用const_cast做强制类型转换
            return iterator(const_cast<list<T> * > (this), head, 0);
        }

        iterator gettail() const {
            return iterator(const_cast<list<T> * > (this), tail, getsize()-1);
        }

        iterator getend() const{
            return iterator(const_cast<list<T> * > (this), nullptr, getsize());
        }

        bool empty() const{
            return size == 0;
        }

        size_t getsize() const{
            return size;
        }

        iterator insert(const iterator & p,const T & v){
            iterator it;
            if(empty()){
                head = tail = new node(v);
                it = getbegin();
            }
            else if (p == getend()){
                node *tmp = new node(v);
                it = iterator(this, tmp, size);
                connect(gettail(), it);
                tail = tmp;
            }
            else if (p == getbegin()){
                node *tmp = new node(v);
                connect(iterator(this, tmp, 0), getbegin());
                head = tmp;
                it = getbegin();
            }
            else{
                iterator tmp = p - 1;
                iterator t = iterator(this, new node(v), p - getbegin());
                cutoff(tmp, p);
                connect(tmp, t), connect(t, p);
                it = t;
            }
            ++size;
            return it;
        }

        iterator erase(const iterator &p){
            iterator it;
            if(p == getbegin()){
                if(p == gettail()) head = tail = nullptr;
                else{
                    head = (p+1);
                    cutoff(p, p+1);
                }
                --size;
                it = getbegin();
            }
            else if(p == gettail()){
                tail = (p - 1);
                cutoff(p-1, p);
                --size;
                it = getend();
            }
            else{
                it = iterator(this, p+1, p-getbegin());
                connect(p-1, p+1);
                --size;
            }
            p.destroy_pos();
            return it;
        }

        size_t getsize(){
            return size;
        }
        
        list * split(const size_t u){  //在第u个位置拆开链表,并返回新链表
            iterator it = getbegin() + u;
            list * ret = new list(it, gettail(), getsize() - u);
            tail = (it-1);
            cutoff(it-1, it);
            size = u;
            return ret;
        }
    };



/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/



template<class T>
class deque {
    list<list<T> > List;
    size_t _size;
public:
	class const_iterator;
	class iterator {
	private:
        typename list<list<T> >::iterator chainlist;
        typename list<T>::iterator sublist;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   if there are not enough elements, iterator becomes invalid
		 * as well as operator-
		 */
        typename list<list<T> >::iterator & chain(){
            return chainlist;
        }

        typename list<T>::iterator & sub(){
            return sublist;
        }

        typename list<list<T> >::iterator const & chain() const{
            return chainlist;
        }

        typename list<T>::iterator const & sub() const{
            return sublist;
        }

        iterator(){}

		iterator(typename list<list<T> >::iterator const & p,typename list<T>::iterator const & q){
            chainlist = p;
            sublist = q;
		}

		iterator operator+(const int &n) const {
            if(n < 0) return *this -(-n);
            int n0 = n;
            iterator tmp = *this;
            if(!tmp.chainlist.available()) return tmp; //主链指向空指针，则直接返回
            //size_t dis = tmp.q - tmp.p->getbegin();
            while(tmp.sublist - tmp.chainlist->getbegin() + n0 >= tmp.chainlist->getsize()){
                n0 -= tmp.chainlist->getsize() - (tmp.sublist - tmp.chainlist->getbegin());
                ++tmp.chainlist;
                if(tmp.chainlist.available()) tmp.sublist = tmp.chainlist->getbegin();
                else{
                    tmp.sublist = (--tmp.chainlist)->getend();
                    return tmp;
                }
            }
            tmp.sublist += n0;
            return tmp;
		}

		iterator operator-(const int &n) const {
            if(n < 0) return *this +(-n);
            int n0 = n;
            iterator tmp = *this;
            if(!tmp.chainlist.available()) return tmp;
            while(tmp.sublist-(tmp.chainlist->getbegin()) - n0 < 0){
                n0 -= tmp.sublist - (tmp.chainlist->getbegin());
                tmp.sublist = (--tmp.chainlist)->getend();
            }
            tmp.sublist -= n0;
            return tmp;
		}

		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.

		size_t calc() const{
            typename list<list<T> >::iterator tmp1 = chainlist;
            typename list<T>::iterator tmp2 = sublist;
            size_t dis = 0;
            while(1){
                if (!(--tmp2).available()) break;
                ++dis;
            }
            while(1){
                if (!(--tmp1).available()) break;
                dis += tmp1->getsize();
            }
            return dis;
        }

		int operator-(const iterator &rhs) const {
            if(chainlist.getroot() != rhs.chainlist.getroot()) throw invalid_iterator();
            return calc() - rhs.calc();
		}

		iterator& operator+=(const int &n) {
            return *this = *this +(n);
		}

		iterator& operator-=(const int &n) {
            return *this = *this -(n);
		}

		/**
		 * TODO iter++
		 */

		iterator& operator++() {
            if(!sublist.available()) throw invalid_iterator();
            if(chainlist->getend() == sublist + 1){
                if(!(chainlist + 1).available()) ++sublist;
                else sublist = (++chainlist)->getbegin();
            }
            else ++sublist;
            return *this;
		}

		iterator operator++(int) {
            if(!sublist.available()) throw invalid_iterator();
            iterator tmp = *this;
            ++*this;
            return tmp;
		}

		/**
		 * TODO ++iter
		 */

		/**
		 * TODO iter--
		 */

		iterator& operator--() {
            if(!(chainlist - 1).available() && sublist == chainlist->getbegin())
                throw invalid_iterator();
            if(chainlist->getbegin() == sublist) sublist = (--chainlist)->gettail();
            else --sublist;
            return *this;
		}

		iterator operator--(int) {
            if(!(chainlist-1).available() && sublist == chainlist->getbegin())
                throw invalid_iterator();
            iterator tmp = *this;
            --*this;
            return tmp;
		}
		/**
		 * TODO --iter
		 */

		/**
		 * TODO *it
		 * 		throw if iterator is invalid
		 */
		T& operator*() const {
            if(sublist.available()) return *sublist;
            else throw invalid_iterator();
		}
		/**
		 * TODO it->field
		 * 		throw if iterator is invalid
		 */
		T* operator->() const {
            return sublist.operator ->();
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
            return chainlist == rhs.chainlist && sublist == rhs.sublist;
		}

		bool operator==(const const_iterator &rhs) const {
            return chainlist == rhs.chain() && sublist == rhs.sub();
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
            return chainlist != rhs.chainlist || sublist != rhs.sublist;
		}

		bool operator!=(const const_iterator &rhs) const {
            return chainlist != rhs.chain() || sublist != rhs.sub();
		}
	};
	class const_iterator {
    private:
        typename list<list<T> >::iterator chainlist;
        typename list<T>::iterator sublist;
        /**
         * TODO add data members
         *   just add whatever you want.
         */
    public:
        /**
         * return a new iterator which pointer n-next elements
         *   if there are not enough elements, iterator becomes invalid
         * as well as operator-
         */
        typename list<list<T> >::iterator & chain(){
            return chainlist;
        }

        typename list<T>::iterator & sub(){
            return sublist;
        }

        typename list<list<T> >::iterator const & chain() const{
            return chainlist;
        }

        typename list<T>::iterator const & sub() const{
            return sublist;
        }

        size_t calc() const{
            typename list<list<T> >::iterator tmp1 = chainlist;
            typename list<T>::iterator tmp2 = sublist;
            size_t dis = 0;
            while((--tmp2).available()){
                ++dis;
            }
            while((--tmp1).available()){
                dis += tmp1->getsize();
            }
            return dis;
        }

        const_iterator operator+(const int &n) const {
            if(n < 0) return *this -(-n);
            int n0 = n;
            const_iterator tmp = *this;
            if(!tmp.chainlist.available()) return tmp;
            while(tmp.sublist - (tmp.chainlist->getbegin()) + n0 >= tmp.chainlist->getsize()){
                n0 -= tmp.chainlist->getsize() - (tmp.sublist - tmp.chainlist->getbegin());
                ++tmp.chainlist;
                if(tmp.chainlist.available()) tmp.sublist = tmp.chainlist->getbegin();
                else{
                    tmp.sublist = (--tmp.chainlist)->getend();
                    return tmp;
                }
            }
            tmp.sublist += n0;
            return tmp;
        }

        const_iterator operator-(const int &n) const {
            if(n < 0) return *this +(-n);
            int n0 = n;
            const_iterator tmp = *this;
            if(!tmp.chainlist.available()) return tmp;
            while(tmp.sublist-(tmp.chainlist->getbegin()) - n0 < 0){
                n0 -= tmp.sublist - (tmp.chainlist->getbegin());
                tmp.sublist = (--tmp.chainlist)->getend();
            }
            tmp.sublist -= n0;
            return tmp;
        }

        // return th distance between two iterator,
        // if these two iterators points to different vectors, throw invaild_iterator.

        int operator-(const const_iterator &rhs) const {
            return calc() - rhs.calc();
        }

        const_iterator& operator+=(const int &n) {
            return *this = *this +(n);
        }

        const_iterator& operator-=(const int &n) {
            return *this = *this -(n);
        }
        /**
         * TODO iter++
         */
        const_iterator operator++(int) {
            if(!sublist.available()) throw invalid_iterator();
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        /**
         * TODO ++iter
         */
        const_iterator& operator++() {
            if(!sublist.available()) throw invalid_iterator();
            if(chainlist->getend() == sublist + 1){
                if(!(chainlist + 1).available()) ++sublist;
                else sublist = (++chainlist)->getbegin();
            }
            else ++sublist;
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            if(!(chainlist - 1).available() && sublist == chainlist->getbegin())
                throw invalid_iterator();
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        /**
         * TODO --iter
         */
        const_iterator& operator--() {
            if(!(chainlist - 1).available() && sublist == chainlist->getbegin())
                throw invalid_iterator();
            if(chainlist->getbegin() == sublist) sublist = (--chainlist)->gettail();
            else --sublist;
            return *this;
        }
        /**
         * TODO *it
         * 		throw if iterator is invalid
         */
        const T& operator*() const {
            return *sublist;
        }
        /**
         * TODO it->field
         * 		throw if iterator is invalid
         */
        const T* operator->() const {
            return sublist.operator ->();
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const {
            return chainlist == rhs.chainlist && sublist == rhs.sublist;
        }

        bool operator==(const const_iterator &rhs) const {
            return chainlist == rhs.chain() && sublist == rhs.sub();
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return chainlist != rhs.chain() || sublist != rhs.sub();
        }

        bool operator!=(const const_iterator &rhs) const {
            return chainlist != rhs.chainlist || sublist != rhs.sublist;
        }
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		const_iterator() {
            // TODO
        }

        const_iterator(const const_iterator &other):chainlist(other.chainlist),sublist(other.sublist){
            // TODO
        }

        const_iterator(const iterator &other) :chainlist(other.chain()),sublist(other.sub()){

            // TODO
        }
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
	};
	/**
	 * TODO Constructors
	 */
	deque(){
        _size = 0;
    }

	deque(const deque &other){
        List = other.List;
        _size = other._size;
	}
	/**
	 * TODO Deconstructor
	 */
	~deque() {}
	/**
	 * TODO assignment operator
	 */

	deque &operator=(const deque &other) {
        if(&other == this) return *this;
        List = other.List;
        _size = other._size;
        return *this;
	}

	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */

	T & at(const size_t &pos) {
        if(pos >= _size || pos < 0)
            throw index_out_of_bound();
        return *(begin() + pos);
	}

	const T & at(const size_t &pos) const {
        if(pos >= _size || pos < 0)
            throw index_out_of_bound();
        return *(begin() + pos);
	}

	T & operator[](const size_t &pos) {
        if(pos >= _size || pos < 0)
            throw index_out_of_bound();
        return *(begin() + pos);
	}

	const T & operator[](const size_t &pos) const {
        if(pos >= _size || pos < 0)
            throw index_out_of_bound();
        return *(begin() + pos);
	}

	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */

	const T & front() const {
        if(!empty()) return *begin();
        throw container_is_empty();
	}

	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */

	const T & back() const {
        if(!empty()) return *(end() - 1);
        throw container_is_empty();
	}

	/**
	 * returns an iterator to the beginning.
	 */

	iterator begin() const {
        if(empty()){
            return iterator(typename list<list<T> >::iterator(const_cast<list<list<T> > *>(&List), nullptr, 0),typename list<T>::iterator(nullptr, nullptr, 0));
        }
        return iterator(List.getbegin(), List.getbegin()->getbegin());
	}

	const_iterator cbegin() const {
        return const_iterator(begin());
	}

	/**
	 * returns an iterator to the end.
	 */

	iterator end() const {
        if(empty()){
            return iterator(typename list<list<T> >::iterator(const_cast<list<list<T> > *>(&List), nullptr, 0),typename list<T>::iterator(nullptr, nullptr, 0));
        }
        return iterator(List.gettail(), List.gettail()->getend());
	}

	const_iterator cend() const {
        return const_iterator(end());
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
        return size() == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
        return _size;
	}
	/**
	 * clears the contents
	 */
	void clear() {
        List.clear_list();
        _size = 0;
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
        if(pos.chain().getroot() != &List) throw invalid_iterator();
        if(empty()){  //如果为空则新建一个子链表块
            list<T> *ret = new list<T>();
            ret->push_back(value); //将value加入链表块
            List.push_back(*ret);  //将子链表块加入主链表
            delete ret; //是否需要delete?
            ++_size;
            return begin();
        }
        if(pos.chain()->getsize() >= 2 * NN){
            list<T> * tmp = pos.chain()->split(NN);
            size_t n0 = pos.sub() - (pos.chain()->getbegin());
            List.insert(pos.chain()+1, *tmp);
            //List.push_back(*tmp);
            //会导致error;
            //注意push_back是外接口,是在最后一个位置(主链表尾部链表的尾部)插入;
            delete tmp;
            if(n0 >= NN){
                n0 -= NN;
                ++pos.chain();
                pos.sub() = pos.chain()->getbegin() + n0;
            }
        }
        ++_size;
        return iterator(pos.chain(), pos.chain()->insert(pos.sub(), value));
    }
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
        if(pos.chain().getroot() != &List || pos == end())
            throw invalid_iterator();
        --_size;
        iterator ret = iterator(pos.chain(), pos.chain()->erase(pos.sub()));
        if(pos.chain()->empty()){
            size_t shift = pos.chain() - List.getbegin();
            typename list<list<T> >::iterator tmp = List.erase(pos.chain());
            if(tmp == List.getend()) ret = end();
            else{
                tmp = List.getbegin() + shift;
                ret = iterator(tmp, tmp->getbegin());
            }
        }
        else if(ret.sub() == ret.chain()->getend() && ret != end()){
            ret.sub() = (++ret.chain())->getbegin();
        }
        return ret;
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
        insert(end(), value);
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
        if(empty()) throw container_is_empty();
        erase(end() - 1);
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
        insert(begin(), value);
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
        if(empty()) throw container_is_empty();
        erase(begin());
	}
};

}

#endif