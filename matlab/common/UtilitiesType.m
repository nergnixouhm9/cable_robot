classdef UtilitiesType
  properties
    data_logger_i = 1;
    frame_i = 1;
    t_interval = 0.001;
    frame_rate = 60;
    sampling_count = 1/(60*0.001);
    error = 0;
    fsolve_options = optimoptions('fsolve','Algorithm',...
      'levenberg-marquardt','FunctionTolerance',1e-8,'MaxFunctionEvaluation',...
      1000000,'MaxIterations',1000000,'OptimalityTolerance',1e-8,...
      'StepTolerance',1e-8);
    fsolve_options_grad = optimoptions('fsolve','Algorithm',...
      'levenberg-marquardt','FunctionTolerance',1e-8,'MaxFunctionEvaluation',...
      1000000,'MaxIterations',1000000,'OptimalityTolerance',1e-8,...
      'display','none','StepTolerance',1e-8,'SpecifyObjectiveGradient',true);
    lsqnonlin_options = optimoptions('lsqnonlin','Algorithm',...
      'levenberg-marquardt','FunctionTolerance',1e-6,'MaxFunctionEvaluation',...
      1000000,'MaxIterations',1000000,'OptimalityTolerance',1e-6,'StepTolerance',1e-6);
    lsqnonlin_options_grad = optimoptions('lsqnonlin','Algorithm',...
      'levenberg-marquardt','FunctionTolerance',1e-6,'MaxFunctionEvaluation',...
      1000000,'MaxIterations',1000000,'OptimalityTolerance',1e-6,...
      'StepTolerance',1e-6,'SpecifyObjectiveGradient',true);
    fmincon_options = optimoptions('fmincon','Algorithm','interior-point',...
      'FunctionTolerance',1e-8,'MaxFunctionEvaluation',1000000,...
      'MaxIterations',1000000,'OptimalityTolerance',1e-8,'StepTolerance',1e-8);
  end
  methods
    function obj = ResetDataLoggerIndex(obj)
      obj.data_logger_i = 1;
    end
    function obj = ResetFrameIndex(obj)
      obj.frame_i = 1;
    end
    function obj = SetTimeInterval(obj,t)
      obj.t_interval = t;
    end
    function obj = SetFrameRate(obj,frame_r)
      obj.frame_rate = frame_r;
      if obj.t_interval > 0
        obj.sampling_count = 1/(obj.t_interval*obj.frame_rate);
      end
    end
  end
end