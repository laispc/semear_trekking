void orientacao ()
{
  distancia= dist(flat,flon,latf,lonf);  
  beta=AchaAlpha(10,0,flat,flon,latf,lonf);
  angCerto=to360(beta - alpha);
  angMeas=to360(angMeas);
  angApontado=to360(angRef-angMeas);// angulo apontado pelo robo
  angDif= to360(angCerto - angApontado); // diferença no plano xy global entre o ang do robo e o ang do proximo ponto.
  if(angDif > 180) angDif -= 360;
}

// será explicado melhor futuramente
float AchaAlpha (float x1,float y1,float lat0,float lon0,float lat1,float lon1)
{ 
  float dlat = lat1-lat0;
  float dlon = lon1-lon0;
  float alpha=0;
  alpha= atan2(dlat,dlon)-atan2(y1,x1);
  alpha=180*alpha/pi;
  alpha=to360(alpha);
  return alpha;
}

// transforma ângulos >360 ou <0 em ângulos entre 0 e 360 
float to360(float ang)
{
  float res=ang;
  if(ang < 0){
    res = 360+ang;
  }
  if(ang>360){
    res=ang-360;
  }
  return res;
}
