
#ifndef IIR_H__
#define IIR_H__

class IIR
{
public:
    float A { 0.9F };
    float B { 1.0F - A };
    float avg { 0.0F };

    IIR( const float A_ ): A { A_ }, B { 1.0F - A_ } {}
    
    //
    // add a sample and return the updated average
    //
    template< typename Num > Num
    add_sample( const Num x )
    {
        const float x_f { static_cast< float >( x ) };
        this->avg = this->A * this->avg + this->B * x_f;
        return static_cast< Num >( this->avg );
    }

};

#endif

