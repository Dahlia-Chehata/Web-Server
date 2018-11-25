#ifndef ATOMIC_RAII_H_
#define ATOMIC_RAII_H_

 /*
 * Resource acquisition is initialization.
 * this is used to increment and decrement atomic
 * variable in constructor and destructor.
 */
class atomic_RAII{
	private:
		std::atomic<uint32_t>& var_;
	public:
		atomic_RAII(std::atomic<uint32_t>& var);
		~atomic_RAII();
};

#endif //ATOMIC_RAII_H_
