#ifndef SRC_SHARED_UTILS_NAMEDFACTORY_HPP_
#define SRC_SHARED_UTILS_NAMEDFACTORY_HPP_

#include <Client/Editor/Tool.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

/**
 * Creates instances by name.
 */
template<typename ProductBase>
class NamedFactory
{
public:

	/**
	 * Attempts to invoke the factory function with the specified name and returns the resulting instance. If no factory
	 * with the specified name exists, returns a null pointer.
	 */
	static std::unique_ptr<ProductBase> createInstance(const std::string & name)
	{
		auto it = instantiators.find(name);

		if (it == instantiators.end())
		{
			return nullptr;
		}

		return std::unique_ptr<ProductBase>(it->second());
	}

	/**
	 * Registers the specified factory function to create potentially subclassed instances of the base class with the
	 * specified name.
	 */
	static void registrateFunction(std::string name, std::function<ProductBase*()> instantiator)
	{
		instantiators.insert(std::make_pair(name, std::move(instantiator)));
	}
	
	/**
	 * Registers the default constructor of the specified subclass as an instantiator with the specified name.
	 */
	template <typename ProductSubclass>
	static void registrate(std::string name)
	{
		registrateFunction(name, [](){return new ProductSubclass;});
	}

private:

	static std::unordered_map<std::string, std::function<ProductBase*()>> instantiators;
};

template<typename ProductBase>
std::unordered_map<std::string, std::function<ProductBase*()>> NamedFactory<ProductBase>::instantiators = {};

#endif
