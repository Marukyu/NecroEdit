#include "Shared/Utils/DataStream.hpp"
#include "Shared/Utils/Endian.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include "Shared/Utils/StrNumCon.hpp"
#include "Shared/Utils/MakeUnique.hpp"

const DataStream::SizeType DataStream::maxFileBufferSize = 4096;

DataStream::DataStream()
{
	myFile = makeUnique<std::fstream>();

	myIsFile = false;
	myIsOutputEnabled = false;
	myIsBuffered = false;

	setIndexSize(4);
	close();
}
DataStream::DataStream(const DataStream & strm)
{
	myIsFile = false;
	myIsOutputEnabled = false;
	myIsBuffered = false;

	*this = strm;
}
DataStream & DataStream::operator=(const DataStream & strm)
{
	close();

	myFileName = strm.myFileName;
	myIsFile = strm.myIsFile;
	myIsOutputEnabled = strm.myIsOutputEnabled;
	myData = strm.myData;
	myIsBuffered = strm.myIsBuffered;
	myBufferStart = strm.myBufferStart;
	myFileSize = strm.myFileSize;

	if (myIsFile)
	{
		if (myIsOutputEnabled)
			openOutFile(myFileName);
		else
			openInFile(myFileName);
	}

	seek(strm.tell());

	return *this;
}
DataStream::DataStream(DataStream && strm)
{
	myIsFile = false;
	myIsOutputEnabled = false;
	myIsBuffered = false;

	*this = std::move(strm);
}
DataStream & DataStream::operator=(DataStream && strm)
{
	myData = std::move(strm.myData);
	myPos = std::move(strm.myPos);
	myFileSize = std::move(strm.myFileSize);

	myFile = std::move(strm.myFile);
	myTempData = std::move(strm.myTempData);
	myFileName = std::move(strm.myFileName);

	myIsBuffered = std::move(strm.myIsBuffered);
	myBufferStart = std::move(strm.myBufferStart);

	myIsFile = std::move(strm.myIsFile);
	myIsOpen = std::move(strm.myIsOpen);
	myIsOutputEnabled = std::move(strm.myIsOutputEnabled);

	myIndexSize = std::move(strm.myIndexSize);

	return *this;
}
DataStream::~DataStream()
{
	flush();
}

DataStream::ListSizeException::ListSizeException(unsigned int size, unsigned int maxSize)
{
	myText = "DataStream: Indexing size (" + cNtoS(size) + ") exceeds maximum allowed value (" + cNtoS(maxSize) + ").";
}

DataStream::ListSizeException::~ListSizeException() throw()
{

}

const char * DataStream::ListSizeException::what() const throw()
{
	return myText.c_str();
}

const char * DataStream::SizerException::what() const throw()
{
	return "DataStream: Invalid indexing size: must be one of 1, 2 or 4.";
}

bool DataStream::openMemory(const void * data, SizeType size)
{
	myTempData.clear();

	myIsFile = false;
	myIsOutputEnabled = false;
	myIsOpen = true;
	myIsBuffered = false;
	myBufferStart = 0;

	myFile->close();

	if (data && size)
		setData(data,size);
	else
		myData.clear();

	seek(0);

	return true;
}
bool DataStream::openMemory(std::vector<char> && data)
{
	myTempData.clear();

	myIsFile = false;
	myIsOutputEnabled = false;
	myIsOpen = true;
	myIsBuffered = false;
	myBufferStart = 0;

	myFile->close();

	myData = std::move(data);

	seek(0);

	return true;
}
bool DataStream::openOutFile(std::string filename, bool buffered)
{
	close();

	myFileName = filename;
	myFile->open(filename.c_str(), std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);

	if (!*myFile)
	{
		return false;
	}

	myIsFile = true;
	myIsOutputEnabled = true;
	myIsOpen = true;
	myIsBuffered = buffered;
	myBufferStart = 0;

	seek(0);

	return true;
}
bool DataStream::openInFile(std::string filename, bool buffered)
{
	close();

	myFileName = filename;
	myFile->open(filename.c_str(), std::ios::binary | std::ios::in);

	if (!*myFile)
	{
		return false;
	}

	myIsFile = true;
	myIsOutputEnabled = false;
	myIsOpen = true;
	myIsBuffered = buffered;
	myBufferStart = 0;

	myFile->seekg(0, std::ios::end);
	myFileSize = myFile->tellg();

	seek(0);

	return true;
}

void DataStream::seek(SizeType pos)
{
	myPos = pos;
}
void DataStream::seekOff(OffsetType off)
{
	myPos += off;
}
void DataStream::seekEnd(SizeType revOff)
{
	myPos = getDataSize() - revOff;
}
DataStream::SizeType DataStream::tell() const
{
	return myPos;
}

void DataStream::clear()
{
	if (myIsFile)
	{
		if (myIsOutputEnabled)
			openOutFile(myFileName);
	}
	else
	{
		myData.clear();
	}
	seek(0);
}

void DataStream::close()
{
	flush();
	openMemory();
	myIsOpen = false;
}

void DataStream::flush()
{
	if (isOpen() && myFile && myIsFile && myIsOutputEnabled && myIsBuffered && !myData.empty())
	{
		myFile->seekp(myBufferStart);
		myFile->write(myData.data(), myData.size());

		myData.clear();
	}
}

bool DataStream::isValid() const
{
	return isOpen() && tell() <= getDataSize();
}

bool DataStream::endReached() const
{
	return !isOpen() || tell() == getDataSize();
}

bool DataStream::hasMoreData() const
{
	return isOpen() && tell() < getDataSize();
}

bool DataStream::isOpen() const
{
	return myIsOpen;
	//return myIsFile || getDataSize() != 0;
}

/*
void DataStream::use16BitSizes(bool enabled)
{
	my16BitSizes = enabled;
}

bool DataStream::using16BitSizes() const
{
	return my16BitSizes;
}

void DataStream::use16BitStrings(bool enabled)
{
	my16BitStrings = enabled;
}

bool DataStream::using16BitStrings() const
{
	return my16BitStrings;
}
*/

const void * DataStream::getData() const
{
	if (myIsFile)
	{
		if (myTempData.empty())
		{
			SizeType size = getDataSize();
			myTempData.resize(size);
			myFile->seekg(0);
			myFile->read(&myTempData[0], size);
		}
		return myTempData.data();
	}
	else
		return myData.data();
}
DataStream::SizeType DataStream::getDataSize() const
{
	if (myIsFile)
		return myFileSize;
	else
		return myData.size();
}
void DataStream::setData(const void * data, SizeType size)
{
	if (myIsFile)
	{
		if (myIsOutputEnabled)
		{
			clear();
			myFile->write((const char *)data, size);
		}
	}
	else
		myData.assign((const char *)data,(const char *)data+size);
	seek(0);
}

void DataStream::addData(const void * data, SizeType bytes)
{
	// open data stream if it is not opened yet.
	if (!isOpen())
		openMemory();

	if (data && (bytes > 0))
	{
		if (myIsFile)
		{
			if (myIsOutputEnabled)
			{
				if (myIsBuffered)
				{
					// first, flush/move buffer if it is disconnected from the current write position.
					if (tell() < myBufferStart || tell() > myBufferStart + myData.size())
					{
						flush();
						myBufferStart = tell();
					}

					SizeType writePos = tell() - myBufferStart;

					// enlarge buffer.
					sf::Int32 sizeIncrease = writePos + bytes - myData.size();

					if (sizeIncrease > 0)
					{
						myData.resize(writePos + bytes);
					}

					//assert(writePos + bytes <= myData.size());

					// append data to buffer.
					std::memcpy(&myData[writePos], data, bytes);

					// flush buffer if it is too big.
					if (myData.size() > maxFileBufferSize)
					{
						flush();
					}
				}
				else
				{
					myFile->seekp(tell());
					myFile->write((const char *)data, bytes);
				}
			}

			myFileSize += bytes;
		}
		else
		{
			// enlarge buffer.
			sf::Int32 sizeIncrease = tell() + bytes - myData.size();

			if (sizeIncrease > 0)
			{
				myData.resize(tell() + bytes);
			}

			//assert (tell() + bytes <= myData.size());

			// append data to buffer.
			std::memcpy(&myData[tell()], data, bytes);
		}

		seekOff(bytes);
	}
}

bool DataStream::extractData(void * target, SizeType bytes)
{
	if (bytes == 0 || target == 0)
		return false;

	if (endReached() || !isValid())
	{
		seekOff(bytes);
		return false;
	}

	if (myIsFile)
	{
		if (myIsBuffered)
		{
			if (bytes > maxFileBufferSize)
			{
				// perform unbuffered read.
				myFile->seekg(tell());
				myFile->read((char *)target, bytes);
			}
			else
			{
				// reset/move buffer if the read operation does not fit.
				if (tell() < myBufferStart || tell() + bytes > myBufferStart + myData.size())
				{
					// expand buffer to max size.
					myData.resize(maxFileBufferSize);

					// read data.
					myBufferStart = tell();
					myFile->seekg(myBufferStart);
					myFile->read(myData.data(), maxFileBufferSize);

					// shrink buffer to actual read data size.
					myData.resize(myFile->gcount());
				}

				//assert(tell() >= myBufferStart);

				SizeType readPos = tell() - myBufferStart;

				// copy data from buffer into target.
				if (readPos < myData.size())
				{
					std::memcpy(target, &myData[readPos], std::min(bytes, myData.size() - readPos) );
				}
			}
		}
		else
		{
			myFile->seekg(tell());
			myFile->read((char *)target, bytes);
		}
	}
	else
	{
		// read (possibly partial) data from buffer.
		if (tell() < myData.size())
		{
			std::memcpy(target, &myData[tell()], std::min(bytes, myData.size() - tell()) );
		}
	}

	seekOff(bytes);
	return true;
}

void DataStream::setIndexSize(sf::Uint8 size)
{
	if (size != 1 && size != 2 && size != 4)
		throw (SizerException());

	myIndexSize = size;
}

sf::Uint8 DataStream::getIndexSize() const
{
	return myIndexSize;
}

void DataStream::addIndexType(sf::Uint32 data)
{
	if (getIndexSize() == 1)
	{
		if (data > std::numeric_limits<sf::Uint8>::max())
			throw (ListSizeException(data, std::numeric_limits<sf::Uint8>::max()));
		*this << (sf::Uint8)data;
	}
	else if (getIndexSize() == 2)
	{
		if (data > std::numeric_limits<sf::Uint16>::max())
			throw (ListSizeException(data, std::numeric_limits<sf::Uint16>::max()));
		*this << (sf::Uint16)data;
	}
	else if (getIndexSize() == 4)
	{
		*this << data;
	}
	else
		throw (SizerException());
}

sf::Uint32 DataStream::extractIndexType()
{
	if (getIndexSize() == 1)
	{
		sf::Uint8 data;
		*this >> data;
		return data;
	}
	else if (getIndexSize() == 2)
	{
		sf::Uint16 data;
		*this >> data;
		return data;
	}
	else if (getIndexSize() == 4)
	{
		sf::Uint32 data;
		*this >> data;
		return data;
	}
	else
		throw (SizerException());
}


bool DataStream::exportToString(std::string & out) const
{
	out.resize(getDataSize());
	if (myIsFile)
	{
		myFile->seekg(0);
		myFile->read(&out[0], out.size());
	}
	else
	{
		std::memcpy(&out[0], &myData[0], out.size());
	}
	return true;
}


bool DataStream::exportToVector(std::vector<char> & out) const
{
	out.resize(getDataSize());
	if (myIsFile)
	{
		myFile->seekg(0);
		myFile->read(&out[0], out.size());
	}
	else
	{
		std::memcpy(&out[0], &myData[0], out.size());
	}
	return true;
}

bool DataStream::importFromString(const std::string & in)
{
	setData(&in[0], in.size());
	return true;
}

/*
std::string DataStream::getDataString() const
{
	std::string ret(myData.begin(), myData.end());
	return ret;
}
void DataStream::setDataString(const std::string & data)
{
	myData.clear();
	myData.assign(data.begin(), data.end());
}
*/

DataStream & DataStream::operator<< (bool data)
{
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (sf::Int8 data)
{
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (sf::Uint8 data)
{
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (sf::Int16 data)
{
	data = n2hs(data);
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (sf::Uint16 data)
{
	data = n2hs(data);
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (sf::Int32 data)
{
	data = n2hl(data);
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (sf::Uint32 data)
{
	data = n2hl(data);
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (sf::Int64 data)
{
	data = n2hll(data);
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (sf::Uint64 data)
{
	data = n2hll(data);
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (float data)
{
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (double data)
{
	addData(&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator<< (const std::string &data)
{
	/*
	// save length.
	if (using16BitStrings())
		*this << (sf::Uint16)data.size();
	else
		*this << (sf::Uint32)data.size();
	*/

	addIndexType(data.size());
	addData(data.data(), data.size());

	return *this;
}
DataStream & DataStream::operator<< (const std::vector<char> &data)
{
	/*
	// save length.
	if (using16BitSizes())
		*this << (sf::Uint16)data.size();
	else
		*this << (sf::Uint32)data.size();
	*/

	addIndexType(data.size());
	addData(data.data(), data.size());

	return *this;
}
DataStream & DataStream::operator<< (const std::wstring &data)
{
	return *this;
}

/*
DataStream & DataStream::operator<< (Modifier mod)
{
	switch (mod) {
	case Modifier16Bit:
		use16BitSizes(true);
		break;

	case Modifier32Bit:
		use16BitSizes(false);
		break;

	case Modifier16BitStr:
		use16BitStrings(true);
		break;

	case Modifier32BitStr:
		use16BitStrings(false);
		break;

	case ModifierClear:
		clear();
		break;
	}
	return *this;
}
DataStream & DataStream::operator>> (Modifier mod)
{
	return *this << mod;
}
*/


DataStream & DataStream::operator>> (bool &data)
{
	extractData((void *)&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator>> (sf::Int8 &data)
{
	extractData((void *)&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator>> (sf::Uint8 &data)
{
	extractData((void *)&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator>> (sf::Int16 &data)
{
	extractData((void *)&data, sizeof(data));
	data = n2hs(data);
	return *this;
}
DataStream & DataStream::operator>> (sf::Uint16 &data)
{
	extractData((void *)&data, sizeof(data));
	data = n2hs(data);
	return *this;
}
DataStream & DataStream::operator>> (sf::Int32 &data)
{
	extractData((void *)&data, sizeof(data));
	data = n2hl(data);
	return *this;
}
DataStream & DataStream::operator>> (sf::Uint32 &data)
{
	extractData((void *)&data, sizeof(data));
	data = n2hl(data);
	return *this;
}
DataStream & DataStream::operator>> (sf::Int64 &data)
{
	extractData((void *)&data, sizeof(data));
	data = n2hll(data);
	return *this;
}
DataStream & DataStream::operator>> (sf::Uint64 &data)
{
	extractData((void *)&data, sizeof(data));
	data = n2hll(data);
	return *this;
}
DataStream & DataStream::operator>> (float &data)
{
	extractData((void *)&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator>> (double &data)
{
	extractData((void *)&data, sizeof(data));
	return *this;
}
DataStream & DataStream::operator>> (std::string &data)
{
	/*
	// get length.
	sf::Uint32 size;
	if (using16BitStrings())
	{
		sf::Uint16 len = 0;
		*this >> len;
		size = len;
	}
	else
	{
		sf::Uint32 len = 0;
		*this >> len;
		size = len;
	}
	data.resize(size);
	*/

	sf::Uint32 size = extractIndexType();
	char * buf = new char[size];
	extractData(buf, size);
	data.assign(buf, size);
	delete[] buf;

	return *this;
}
DataStream & DataStream::operator>> (std::vector<char> &data)
{
	/*
	// get length.
	sf::Uint32 size;
	if (using16BitSizes())
	{
		sf::Uint16 len = 0;
		*this >> len;
		size = len;
	}
	else
	{
		sf::Uint32 len = 0;
		*this >> len;
		size = len;
	}
	data.resize(size);
	*/

	data.resize(extractIndexType());
	extractData(&data[0], data.size());

	return *this;
}
DataStream & DataStream::operator>> (std::wstring &data)
{
	return *this;
}
