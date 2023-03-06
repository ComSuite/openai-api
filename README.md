# OpenAI-API C++ Client

The OpenAI-API C++ client is a C++ library to interact with the OpenAI API. This library provides a simple interface to send requests to the OpenAI API and handle the responses. The library is built on top of the C++ REST SDK and OpenCV.

# Getting Started
To get started with the OpenAI-API C++ client, you will need to have the following dependencies installed on your system:

cpprest [link] https://github.com/microsoft/cpprestsdk
OpenCV

You can install the C++ REST SDK and OpenCV using your package manager:

	sudo apt-get install libcpprest-dev

Once you have the dependencies installed, you can clone the OpenAI-API C++ client repository to your local machine:

	git clone https://github.com/ComSuite/openai-api-cpp.git

# Usage
To use the OpenAI-API C++ client, you will need to set your OpenAI API key as an environment variable:

export OPENAI_API_KEY=YOUR_API_KEY

Alternatively, you can set the API key programmatically by calling the set_api_key function:

	#include "openai_api.h"

	int main() {
		openai_api::set_api_key("YOUR_API_KEY");

		// ...
	}

You can then use the OpenAI-API C++ client to send requests to the OpenAI API. For example, to generate text using the GPT-3 model, you can use the generate_text function:

	#include "openai_api.h"

	int main() {
		std::string prompt = "Once upon a time";
		std::string response = openai_api::generate_text(prompt);

		// ...
	}

# Contributing
We welcome contributions to the OpenAI-API C++ client! If you find a bug or have a feature request, please open an issue on GitHub. If you would like to contribute code, please fork the repository and submit a pull request.

# License
The OpenAI-API C++ client is licensed under the MIT License. See the LICENSE file for more information.