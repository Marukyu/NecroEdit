
template<typename T> DataStream & DataStream::operator<< (const sf::Vector2<T> & data)
{
	return *this << data.x << data.y;
}
template<typename T> DataStream & DataStream::operator>> (sf::Vector2<T> & data)
{
	return *this >> data.x >> data.y;
}

template<typename T> DataStream & DataStream::operator<< (const std::vector<T> & data)
{
	addIndexType(data.size());

	for (typename std::vector<T>::const_iterator it = data.begin(); it != data.end(); ++it)
		*this << *it;

	return *this;
}
/*
template<> DataStream & DataStream::operator<< (const std::vector<char> & data)
{
	addIndexType(data.size());
	addData(data.data(), data.size());

	return *this;
}
*/
template<typename T> DataStream & DataStream::operator<< (const std::list<T> & data)
{
	addIndexType(data.size());

	for (typename std::list<T>::const_iterator it = data.begin(); it != data.end(); ++it)
		*this << *it;

	return *this;
}
template<typename T1, typename T2> DataStream & DataStream::operator<< (const std::pair<T1,T2> & data)
{
	return *this << data.first << data.second;
}
template<typename TK, typename TD> DataStream & DataStream::operator<< (const std::map<TK,TD> & data)
{
	addIndexType(data.size());

	for (typename std::map<TK,TD>::const_iterator it = data.begin(); it != data.end(); ++it)
		*this << *it;

	return *this;
}
template<typename T> DataStream & DataStream::operator>> (std::vector<T> & data)
{
	sf::Uint32 size = extractIndexType();

	if (isValid())
	{
		data.resize(size);

		for (sf::Uint32 i = 0; i < size; ++i)
		{
			T entry;
			*this >> entry;
			data[i] = entry;
		}
	}
	else
	{
		data.clear();
	}

	return *this;
}
/*
template<> DataStream & DataStream::operator>> (std::vector<char> & data)
{
	sf::Uint32 size = extractIndexType();

	if (isValid())
	{
		data.resize(size);
		extractData(data.data(), data.size());
	}
	else
	{
		data.clear();
	}

	return *this;
}
*/

template<typename T> DataStream & DataStream::operator>> (std::list<T> & data)
{
	data.clear();
	sf::Uint32 size = extractIndexType();

	if (isValid())
	{
		for (sf::Uint32 i = 0; i < size; ++i)
		{
			T entry;
			*this >> entry;
			data.push_back(entry);
		}
	}

	return *this;
}
template<typename T1, typename T2> DataStream & DataStream::operator>> (std::pair<T1,T2> & data)
{
	return *this >> data.first >> data.second;
}
template<typename TK, typename TD> DataStream & DataStream::operator>> (std::map<TK,TD> & data)
{
	data.clear();
	sf::Uint32 size = extractIndexType();

	if (isValid())
	{
		for (sf::Uint32 i = 0; i < size; ++i)
		{
			std::pair<TK,TD> entry;
			*this >> entry;
			data.insert(entry);
		}
	}

	return *this;
}


template<typename T>
DataStream::Sizer<1,T> DataStream::Sizer8(T & data)
{
	return Sizer<1,T>(data);
}

template<typename T>
DataStream::Sizer<2,T> DataStream::Sizer16(T & data)
{
	return Sizer<2,T>(data);
}

template<typename T>
DataStream::Sizer<4,T> DataStream::Sizer32(T & data)
{
	return Sizer<4,T>(data);
}

template<typename T>
DataStream::CSizer<1,T> DataStream::Sizer8(const T & data)
{
	return CSizer<1,T>(data);
}

template<typename T>
DataStream::CSizer<2,T> DataStream::Sizer16(const T & data)
{
	return CSizer<2,T>(data);
}

template<typename T>
DataStream::CSizer<4,T> DataStream::Sizer32(const T & data)
{
	return CSizer<4,T>(data);
}

template<sf::Uint8 size, typename T>
DataStream & DataStream::operator<< (Sizer<size,T> data)
{
	return *this << CSizer<size,T>(data.myData);
}

template<sf::Uint8 size, typename T>
DataStream & DataStream::operator<< (CSizer<size,T> data)
{
	sf::Uint8 sizeSave = getIndexSize();
	setIndexSize(size);
	*this << data.myData;
	setIndexSize(sizeSave);
	return *this;
}

template<sf::Uint8 size, typename T>
DataStream & DataStream::operator>> (Sizer<size,T> data)
{
	sf::Uint8 sizeSave = getIndexSize();
	setIndexSize(size);
	*this >> data.myData;
	setIndexSize(sizeSave);
	return *this;
}


template<typename T> void DataStream::skip()
{
	T discard;
	*this >> discard;
}
template<typename T> void DataStream::peek(T &data)
{
	sf::Uint32 rwPos = tell();
	*this >> data;
	seek(rwPos);
}
template<typename T> DataStream & DataStream::operator>>= (T &data)
{
	peek(data);
	return *this;
}
