#include <functional>
#include <random>

class random_int
{
public:
   random_int(int low, int high)
      :r(std::bind(std::uniform_int_distribution<>(low,high),std::default_random_engine())){}

   double operator()(){ return r(); }

private:
   std::function<int()> r;
};


#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <vector>

typedef float NUMBER;
typedef cv::Point_<NUMBER> POINT;

template < class NUMBER >
NUMBER distance( const NUMBER& x1, const NUMBER& y1, const NUMBER& x2, const NUMBER& y2 )
{
   return std::hypot(x1-x2,y1-y2);
}

void nearest( const POINT& p, const std::vector< POINT >& points, POINT& n, std::vector< POINT >& remainders  )
{
   POINT::value_type min_d = std::numeric_limits<POINT::value_type>::max();
   size_t j = 0;
   for ( size_t i = 0; i < points.size(); i++ ) {
      POINT::value_type d = distance(p.x,points[i].x,p.y,points[i].y);
      if ( d < min_d && p != points[i] ) {
         min_d = d;
         n = points[i];
         j = i;
      }
   }
   remainders = points;
   remainders.erase(remainders.begin()+j);
}

// http://math.stackexchange.com/a/568565/10799
template < class POINT >
typename POINT::value_type y_at_perpendicular_bisector(const POINT& p1, const POINT& p2, typename POINT::value_type x )
{
   return (static_cast<typename POINT::value_type>(1)/static_cast<typename POINT::value_type>(2)*(std::pow(p1.x,2)+std::pow(p1.y,2)-std::pow(p2.x,2)-std::pow(p2.y,2))-(p1.x-p2.x)*x)/(p1.y-p2.y);
}

int main(int argc, const char* argv[])
{
   const size_t w = 400;
   const size_t h = 400;
   cv::Mat image(h,w,CV_8UC3);
   image = cv::Scalar(0,0,0);
   random_int rd(0,std::min(h,w));



   std::vector< POINT > points;

   POINT center(h/2,w/2);
   POINT::value_type min_d = std::numeric_limits<POINT::value_type>::max();
   size_t my = 0;

   for ( size_t i = 0; i < 10; i++ ) {
      points.push_back( POINT( rd(), rd() ) );

      image.at<cv::Vec3b>(points.back()) = cv::Vec3b(255,255,255);

      POINT::value_type d = distance(points.back().x,points.back().y,center.x,center.y);
      if ( d < min_d ) {
         my = i;
         min_d = d;
      }
   }



   POINT myp(points[my]);

   cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
   cv::circle(image,myp,3,cv::Scalar(0,255,0));


   POINT n;
   std::vector< POINT > r( points );

   while ( !r.empty() ) {
      nearest(myp,points,n,r);
      cv::circle(image,n,3,cv::Scalar(0,255,255));
      cv::line(image,myp,n,cv::Scalar(255,255,0));
      cv::line(image,cv::Point(0,y_at_perpendicular_bisector(myp,n,0)),cv::Point(w,y_at_perpendicular_bisector(myp,n,w)),cv::Scalar(0,0,255));
      points = r;
   }

   cv::imshow( "Display window", image ); 
   cv::waitKey(0);
   return 0;
}
