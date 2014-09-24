#include <putki/config.h>
#include <datacontainer/datacontainer.h>
#include <iostream>
#include <map>
#include <fstream>

#define DC_DEBUG(x) { std::cout << "datacontainer:" << x << std::endl; }

namespace datacontainer
{
	struct loaded_data_internal : loaded_data
	{
		outki::DataContainer *source;
		bool allocated;
		int refcount;
	};

	typedef std::map<outki::DataContainer*, loaded_data_internal*> LoadMap;
	LoadMap s_loaded;

	loaded_data* load(outki::DataContainer *container, bool block_until_loaded)
	{
		LoadMap::iterator i = s_loaded.find(container);
		if (i != s_loaded.end())
		{
			DC_DEBUG("Had this loaded already, returning pointer to old record")
			i->second->refcount++;
			return i->second;
		}

		loaded_data_internal *nr = new loaded_data_internal();
		nr->allocated = false;
		nr->refcount = 1;
		nr->source = container;

		outki::DataContainerOutput *output = container->Output;
		DC_DEBUG("Loading data container with output " << output);

		if (!output)
		{
			DC_DEBUG("Data was embedded");
			nr->data = container->Bytes;
			nr->size = (size_t) container->Bytes_size;
			return nr;
		}
		else if (outki::DataContainerOutputFile *file = output->exact_cast<outki::DataContainerOutputFile>())
		{
			char real_path[512];
			putki::format_file_path(file->FilePath, real_path);
			DC_DEBUG("Loading file [" << real_path << "]");

			std::ifstream in(real_path, std::ios::binary);
			if (!in.good())
			{
				DC_DEBUG("Failed to open file [" << real_path << "]!");
				delete nr;
				return 0;
			}

			in.seekg(0, std::ios::end);
			std::streamoff filesize = in.tellg();

			nr->data = new unsigned char[(unsigned long)filesize];
			nr->size = (size_t) filesize;
			nr->allocated = true;
			in.seekg(0, std::ios_base::beg);
			in.read((char*)nr->data, filesize);
			in.close();
			return nr;
		}
		else
		{
			DC_DEBUG("Data was not embedded");
			return 0;
		}
	}

	void release(loaded_data *d)
	{
		loaded_data_internal *record = (loaded_data_internal*) d;
		--record->refcount;
		DC_DEBUG("Refcount is now " << record->refcount);
	}

}
