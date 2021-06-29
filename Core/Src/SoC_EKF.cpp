/*
 * SoC_EKF.cpp
 *
 *  Created on: Jun 22, 2021
 *      Author: jan
 */



#include <SoC_EKF.hpp>

#include <math.h>
void SoC_EKF::set_update_matrix()
{
    _State_vector[ttc1] = 0;
    _State_vector[ttc2] = 0;

    _et1 = expf(-_Ts / _battery.battery_equivalent_model[ttc1]);
    _et2 = expf(-_Ts / _battery.battery_equivalent_model[ttc2]);

    _A_matrix[SoC] = 1;
    _A_matrix[ttc1] = _et1;
    _A_matrix[ttc2] = _et2;

    _B_matrix[SoC] = - _Ts/_battery.battery_equivalent_model[Qnom]/60/60;
    _B_matrix[ttc1] = _battery.battery_equivalent_model[Rttc1]*(1 - _et1);
    _B_matrix[ttc2] = _battery.battery_equivalent_model[Rttc2]*(1 - _et2);

    _P_matrix[SoC] = 0.0005;
    _P_matrix[ttc1] = 0.05;
    _P_matrix[ttc2] = 0.05;

    _Q_matrix[SoC] = _P_matrix[SoC] / 5;
    _Q_matrix[ttc1] = _P_matrix[ttc1] / 5;
    _Q_matrix[ttc2] = _P_matrix[ttc2] / 5;

    _R_coeff = 50000;
}

void SoC_EKF::update(const float &Current, const float &Voltage)
{
    // Predict
    _State_vector[SoC] =  _State_vector[SoC] * _A_matrix[SoC] + Current * _B_matrix[SoC];
    _State_vector[ttc1] =  _State_vector[ttc1] * _A_matrix[ttc1] + Current * _B_matrix[ttc1];
    _State_vector[ttc2] =  _State_vector[ttc2] * _A_matrix[ttc2] + Current * _B_matrix[ttc2];

    // Covariance
    _P_matrix[SoC] = _A_matrix[SoC] * _A_matrix[SoC] * _P_matrix[SoC] + _Q_matrix[SoC];
    _P_matrix[ttc1] = _A_matrix[ttc1] * _A_matrix[ttc1] * _P_matrix[ttc1] + _Q_matrix[ttc1];
    _P_matrix[ttc2] = _A_matrix[ttc2] * _A_matrix[ttc2] * _P_matrix[ttc2] + _Q_matrix[ttc2];

    // Predict Opec circiut voltage single call voltage based on SoC
    float Voltage_predict_OCV = horner(_battery.battery_ocv_poli, SOC_OCV_poli_coeff_lenght, &_State_vector[SoC]);

    float Voltage_predict = Voltage_predict_OCV - _State_vector[Vttc1]/static_cast<float>(_battery.cell_in_parallel)
                                                - _State_vector[Vttc2]/static_cast<float>(_battery.cell_in_parallel)
                                                - (Current) * _battery.battery_equivalent_model[Rs]*static_cast<float>(_battery.cell_in_series);

    float innovation = (Voltage) - Voltage_predict;

    float H = horner(_battery.battery_d_ocv_poli, SOC_OCV_poli_coeff_lenght - 1, &_State_vector[SoC]);

    float S = H *  _P_matrix[SoC] * H + _R_coeff;

    float Kf = _P_matrix[SoC] * H * (1/S);

    _State_vector[SoC] += Kf*innovation;

    _P_matrix[SoC] = (1 - Kf*H) * _P_matrix[SoC];

    // clamping SoC to <0,1>
    if( _State_vector[SoC] < 0.00f )      _State_vector[SoC]  = 0;
    else if(_State_vector[SoC] > 1.00f ) _State_vector[SoC]  = 1;
}

void SoC_EKF::set_battery_equivalent_model(const float *battery_model){
    std::copy(battery_model, battery_model+8, _battery.battery_equivalent_model);
}

void SoC_EKF::set_Initial_SoC(const float &aSoC){
    _State_vector[SoC] = aSoC;
}

void SoC_EKF::set_Time_Sampling(float Ts){
    _Ts = Ts;
}

void SoC_EKF::set_battery_configuration(unsigned int &&s, unsigned int &&p){
    _battery.cell_in_parallel = p;
    _battery.cell_in_series = s;
}

void SoC_EKF::set_battery_ocv_polinomial(const float *battery_ocv, unsigned int number_of_coef){

    std::copy(battery_ocv, battery_ocv + number_of_coef, _battery.battery_ocv_poli);

    for(unsigned int ocv_poly_iter = 0; ocv_poly_iter < number_of_coef - 1; ocv_poly_iter++){
        _battery.battery_d_ocv_poli[ocv_poly_iter] = static_cast<float>(number_of_coef - ocv_poly_iter - 1) * battery_ocv[ocv_poly_iter];
    }
}


//void debug_display(){
//    std::cout<<"Voltage "<< _battery.cell_in_parallel <<std::endl;
//    std::cout<<"innowation: "<<innovation<<std::endl;
//    std::cout<<"H: "<<H<<std::endl;
//    std::cout<<"S: "<<S<<std::endl;
//    std::cout<<"Kf: "<<Kf <<std::endl;
//    std::cout<<"SoC: "<<State_vector[0]<<std::endl;
//}

 [[nodiscard]] float horner(const float *arry, unsigned int n, const float *x)
{
    float s = 0;
    for(unsigned int i = 0; i < n ; i++){
        s = s*(*x) + arry[i];
    }
    return s;
}
