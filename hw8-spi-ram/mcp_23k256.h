
#ifndef MCP_23K256_H
#define MCP_23K256_H

#include "util.h"
#include "spi_util.h"

#include <cstdint>
#include <cstddef>
#include <array>

namespace MCP_23K256
{

    namespace Instr
    {
        static constexpr std::uint8_t
            READ  { 0b0000'0011 },
            WRITE { 0b0000'0010 },
            RDSR  { 0b0000'0101 },
            WRSR  { 0b0000'0001 }
            ;
    };

    // mode bits offset in status register instruction
    static std::size_t STATUS_MODE_OFS { 6 }; 
    enum class Mode: std::uint8_t
    {
        BYTE = 0b00,
        PAGE = 0b10,
        SEQ  = 0b01,
    };

    static constexpr std::size_t PAGE_ID_LEN { 10 }, PAGE_OFS_LEN { 5 };
    static constexpr std::uint16_t
        PAGE_ID_MSK  { ( 0b1 << PAGE_ID_LEN  ) - 1 },
        PAGE_OFS_MSK { ( 0b1 << PAGE_OFS_LEN ) - 1 };

    static inline void init( void );

    static inline void setmode( const uint pin, const Mode mode );
    static inline Mode getmode( const uint pin );

    static inline std::uint16_t make_addr( const std::uint16_t page_id_, const std::uint16_t page_ofs_ );
    static inline void parse_addr( const std::uint16_t addr, std::uint16_t &page_id, std::uint16_t &page_ofs );

    static inline void begin_seqwrite( const uint pin, const std::uint16_t addr );
    static inline void seqwrite_u16( const std::uint16_t val );
    static inline void end_seqwrite( const uint pin );

    static inline void begin_seqread( const uint pin, const std::uint16_t addr );
    static inline std::uint16_t seqread_u16( void );
    static inline void end_seqread( const uint pin );


}

static inline void
MCP_23K256::init( void )
{
    // pass
}

static inline void
MCP_23K256::setmode( const uint pin, const MCP_23K256::Mode mode )
{
    std::uint8_t mode_bits { static_cast< std::uint8_t >( mode ) << STATUS_MODE_OFS };
    mode_bits |= 0b1; // set write enable bit

    SPIUtil::cs_select( pin );
    spi_write_blocking( SPI_PORT, &Instr::WRSR, 1 );
    spi_write_blocking( SPI_PORT, &mode_bits, 1 );
    SPIUtil::cs_deselect( pin );
}

static inline MCP_23K256::Mode
MCP_23K256::getmode( const uint pin )
{
    std::uint8_t buf {};
 
    SPIUtil::cs_select( pin );
    spi_write_blocking( SPI_PORT, &Instr::RDSR, 1 );
    spi_read_blocking( SPI_PORT, 0U, &buf, 1 );
    SPIUtil::cs_deselect( pin );

    return static_cast< Mode >( ( buf >> STATUS_MODE_OFS ) & 0b11 );
}

static inline std::uint16_t
MCP_23K256::make_addr( const std::uint16_t page_id_, const std::uint16_t page_ofs_ )
{
    const std::uint16_t
        page_id  { page_id_  & PAGE_ID_MSK },
        page_ofs { page_ofs_ & PAGE_OFS_MSK };
    return { ( page_id << PAGE_OFS_LEN ) | page_ofs };
}

static inline void
MCP_23K256::parse_addr(
    const std::uint16_t addr,
    std::uint16_t &page_id, std::uint16_t &page_ofs
)
{
    page_id = ( addr >> PAGE_OFS_LEN ) & PAGE_ID_MSK;
    page_ofs = addr & PAGE_OFS_MSK;
}

static inline void
MCP_23K256::begin_seqwrite( const uint pin, const std::uint16_t addr )
{
    SPIUtil::cs_select( pin );
    const std::array< std::uint8_t, 2 > addrbuf { Util::u8s_from_u16( addr ) };

    spi_write_blocking( SPI_PORT, &Instr::WRITE, 1 );
    spi_write_blocking( SPI_PORT, addrbuf.data(), 2 );
}

static inline void
MCP_23K256::seqwrite_u16( const std::uint16_t val )
{
    const std::array< std::uint8_t, 2 > buf { Util::u8s_from_u16( val ) };
    spi_write_blocking( SPI_PORT, buf.data(), 2 );
}

static inline void
MCP_23K256::end_seqwrite( const uint pin )
{
    SPIUtil::cs_deselect( pin );
}

static inline void
MCP_23K256::begin_seqread( const uint pin, const std::uint16_t addr )
{
    SPIUtil::cs_select( pin );
    const std::array< std::uint8_t, 2 > addrbuf { Util::u8s_from_u16( addr ) };

    spi_write_blocking( SPI_PORT, &Instr::READ, 1 );
    spi_write_blocking( SPI_PORT, addrbuf.data(), 2 );
}

static inline std::uint16_t
MCP_23K256::seqread_u16( void )
{
    std::array< std::uint8_t, 2 > buf {};
    spi_read_blocking( SPI_PORT, 0U, buf.data(), 2 );
    return Util::u16_from_u8s( buf.data() );
}

static inline void
MCP_23K256::end_seqread( const uint pin )
{
    SPIUtil::cs_deselect( pin );
}

#endif 
