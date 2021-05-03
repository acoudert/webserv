#ifndef _ERRORGENERATOR_HPP_
#define _ERRORGENERATOR_HPP_

#include "IGenerator.hpp"
#include <sys/types.h>
#include <string>
#include <sys/select.h>

struct errMapper {
	unsigned short code;
	const char *begin;
	const char *end;
};

extern const struct errMapper mapperList[];

extern const char _binary_assets_200_html_start[];
extern const char _binary_assets_200_html_end[];
extern const char _binary_assets_201_html_start[];
extern const char _binary_assets_201_html_end[];
extern const char _binary_assets_301_html_start[];
extern const char _binary_assets_301_html_end[];
extern const char _binary_assets_400_html_start[];
extern const char _binary_assets_400_html_end[];
extern const char _binary_assets_401_html_start[];
extern const char _binary_assets_401_html_end[];
extern const char _binary_assets_403_html_start[];
extern const char _binary_assets_403_html_end[];
extern const char _binary_assets_404_html_start[];
extern const char _binary_assets_404_html_end[];
extern const char _binary_assets_405_html_start[];
extern const char _binary_assets_405_html_end[];
extern const char _binary_assets_408_html_start[];
extern const char _binary_assets_408_html_end[];
extern const char _binary_assets_413_html_start[];
extern const char _binary_assets_413_html_end[];
extern const char _binary_assets_418_html_start[];
extern const char _binary_assets_418_html_end[];
extern const char _binary_assets_500_html_start[];
extern const char _binary_assets_500_html_end[];
extern const char _binary_assets_501_html_start[];
extern const char _binary_assets_501_html_end[];
extern const char _binary_assets_505_html_start[];
extern const char _binary_assets_505_html_end[];

class ErrorGenerator : public IGenerator {
 private:
  const char *begin;
  const char *end;
  size_t size;

  ErrorGenerator ();
 public:
  explicit ErrorGenerator (int code);
  ErrorGenerator (const ErrorGenerator &x);
  ErrorGenerator &operator= (const ErrorGenerator &x);

  ssize_t Read (char *buf, size_t s, fd_set const *selectedFDs);
  bool fullyHandled () const;
  ssize_t getSize (fd_set const *selectedFDs, int *toCloseFds);
  IGenerator *clone () const;
  std::string genGetMime () const;
  void destroy ();
};

#endif //_ERRORGENERATOR_HPP_
