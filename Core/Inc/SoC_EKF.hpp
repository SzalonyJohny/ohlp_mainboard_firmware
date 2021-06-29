/*
 * SoC_EKF.hpp
 *
 *  Created on: Jun 22, 2021
 *      Author: jan
 */

#ifndef SOC_EKF_HPP_
#define SOC_EKF_HPP_

#include <array>
#include <math.h>

const unsigned int SOC_OCV_poli_coeff_lenght = 9;

enum batt{
    Rs,
    ttc1,
    ttc2,
    Rttc1,
    Rttc2,
    Cttc1,
    Cttc2,
    Qnom
};


enum state_vector{
    SoC,
    Vttc1,
    Vttc2
};

float horner(const float *arry,unsigned int n, const float *x);

struct Battery_characteristic{
    unsigned int cell_in_parallel = 1;
    unsigned int cell_in_series = 1;
    float battery_equivalent_model[8];
    float battery_ocv_poli[SOC_OCV_poli_coeff_lenght];
    float battery_d_ocv_poli[SOC_OCV_poli_coeff_lenght];
};




class SoC_EKF
{
private:
    Battery_characteristic _battery;
    float _et1;
	float _et2;
    float _Ts;

    float _A_matrix[3];
    float _B_matrix[3];
    float _P_matrix[3];
    float _R_coeff;
    float _Q_matrix[3];
    float _State_vector[3];


public:
    SoC_EKF() = default;

    /* Battery model represented in Two Time Constatant model */
    /*  { Rs[Ohms], ttc1[s], ttc2, Rttc1[Ohms], Rttc2, Cttc1[F], Cttc2, Qnominal[Ah]} */
    void set_battery_equivalent_model(const float *battery_model);

    /* Battery OCV curve represented by polynomial [0] -> highest order coeff */
    /* and calc derative of ocv polinomial  */
    void set_battery_ocv_polinomial(const float *battery_ocv, unsigned int number_of_coef);

    /* Define battery configuration in eg. 1s2p -> s = 1, p = 2 */
    void set_battery_configuration(unsigned int &&s, unsigned int &&p);

    /* Constant Time sampling*/
    void set_Time_Sampling(float Ts);

    void set_Initial_SoC(const float &aSoC);

    void set_update_matrix();
    void update(const float &Current, const float &Voltage);

    [[nodiscard]] float get_SoC(){return _State_vector[SoC];};
};




#endif /* SOC_EKF_HPP_ */
