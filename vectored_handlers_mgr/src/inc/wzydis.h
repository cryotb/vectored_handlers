#pragma once

namespace zy
{
	class Decoder {
	public:
		Decoder() = default;
		Decoder(ZydisMachineMode machineMode, ZydisAddressWidth addressWidth)
		{
			ZydisDecoderInit(&m_Instance, machineMode, addressWidth);
		}

		~Decoder() = default;

		ZyanStatus Decode(const void* buffer, ZyanUSize length, ZydisDecodedInstruction* instruction);
	private:
		ZydisDecoder m_Instance{};
	};

	inline ZyanStatus Decoder::Decode(const void* buffer,
		ZyanUSize length, ZydisDecodedInstruction* instruction)
	{
		return ZydisDecoderDecodeBuffer(&m_Instance, buffer, length, instruction);
	}

	inline bool DisasmBuffer(std::uint8_t* buffer, std::size_t length, std::vector<ZydisDecodedInstruction>& list)
	{
		auto decoder = Decoder(ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);

		ZyanUSize offset = 0;
		ZydisDecodedInstruction instr{};

		do
		{
			if (!ZYAN_SUCCESS(decoder.Decode(&buffer[offset], length - offset, &instr)))
				break;

			list.emplace_back(instr);
			offset += instr.length;
		} while (true);

		return true;
	}

	inline std::size_t GetInstrList(std::uint8_t* data, std::size_t length,
		ZydisDecodedInstruction* buffer_ptr, std::size_t buffer_length)
	{
		auto decoder = Decoder(ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);

		ZyanU64 runtime_address = 0x0;
		ZyanUSize offset = 0;

		ZydisDecodedInstruction instruction{};

		SIZE_T write_index = 0;

		while (ZYAN_SUCCESS(decoder.Decode(data + offset, length - offset, &instruction)))
		{
			if (write_index >= buffer_length)
				break;

			buffer_ptr[write_index] = instruction;

			offset += instruction.length;
			runtime_address += instruction.length;
			++write_index;
		}

		return write_index;
	}
}
