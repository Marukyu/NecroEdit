#ifndef GUI2_INTERNAL_SIGNAL_HPP
#define GUI2_INTERNAL_SIGNAL_HPP

namespace gui2
{

/**
 * utility class to notify widgets of important events recursively or directly.
 */
class Signal
{
public:

	enum Type
	{
		FontChanged,		// sent to all widgets when any of the system fonts is changed/removed.
		TextureChanged,	 // sent to all widgets when the system texture is updated.
	};
};

} // namespace gui2

#endif
