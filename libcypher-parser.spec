# Copyright 2016, Chris Leishman (http://github.com/cleishm)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
Summary: Parsing library for the Cypher query language
Name: libcypher-parser
Version: 0.5.4
Release: 1%{?dist}
Group: System Environment/Libraries
License: Apache-2.0
URL: https://github.com/cleishm/libcypher-parser
Source0: https://github.com/cleishm/libcypher-parser/releases/download/%{version}/%{name}-%{version}.tar.gz
BuildRequires: doxygen pkgconfig

%description
libcypher-parser provides efficient parsing of Cypher into an Abstract Syntax
Tree (AST) form, using a parsing expression grammar that is equivalent to that
used in the Neo4j graph database.

%define sover 8
%define libname %{name}%{sover}

%prep
%setup -q

%build
%configure --disable-static
make
make doc

%check
make check

%install
%make_install
rm $RPM_BUILD_ROOT%{_libdir}/*.la

#------------------------------------------------------------------------------

%package -n %{libname}
Summary: Parsing library for the Cypher query language
Group: System Environment/Libraries
Provides: %{name}

%description -n %{libname}
ibcypher-parser provides efficient parsing of Cypher into an Abstract Syntax
Tree (AST) form, using a parsing expression grammar that is equivalent to that
used in the Neo4j graph database.

%post -n %{libname} -p /sbin/ldconfig
%postun -n %{libname} -p /sbin/ldconfig

%files -n %{libname}
%defattr(-, root, root)
%doc README.md
%{_libdir}/*.so.*


#------------------------------------------------------------------------------

%package -n %{name}-devel
Summary: Development files for %{name}
Group: Development/Libraries
Requires: %{libname}%{?_isa} = %{version}-%{release}

%description -n %{name}-devel
ibcypher-parser provides efficient parsing of Cypher into an Abstract Syntax
Tree (AST) form, using a parsing expression grammar that is equivalent to that
used in the Neo4j graph database.

This package contains the development files (headers, etc).

%files -n %{name}-devel
%defattr(-, root, root)
%{_includedir}/cypher-parser.h
%{_libdir}/*.so
%{_libdir}/pkgconfig/*.pc

#------------------------------------------------------------------------------

%package -n %{name}-devel-doc
Summary: Development documentation for %{name}
Group: Development/Libraries
BuildArch: noarch

%description -n %{name}-devel-doc
ibcypher-parser provides efficient parsing of Cypher into an Abstract Syntax
Tree (AST) form, using a parsing expression grammar that is equivalent to that
used in the Neo4j graph database.

This package contains the API documentation that is also available on the
libcypher-parser website (https://github.com/cleishm/libcypher-parser).

%files -n %{name}-devel-doc
%defattr(-, root, root)
%doc doc/html

#------------------------------------------------------------------------------

%package -n cypher-lint
Summary: Lint tool for the Cypher query language
Group: Applications/Databases

%description -n cypher-lint
cypher-lint performs correctness checking of Cypher language input, and can be
used to output the Abstract Syntax Tree (AST) of the parsed input.

%files -n cypher-lint
%defattr(-, root, root)
%{_bindir}/cypher-lint
%{_mandir}/man1/cypher-lint.1*

#------------------------------------------------------------------------------

%changelog
* Thu Sep 07 2017 Chris Leishman <chris@leishman.org> - 0.5.4-1
- New upstream release
* Thu May 25 2017 Chris Leishman <chris@leishman.org> - 0.5.3-1
- New upstream release
* Sat Sep 24 2016 Chris Leishman <chris@leishman.org> - 0.5.2-1
- New upstream release
* Mon Aug 15 2016 Chris Leishman <chris@leishman.org> - 0.5.1-1
- New upstream release
* Wed Aug 10 2016 Chris Leishman <chris@leishman.org> - 0.5.0-1
- New upstream release
* Mon Jul 18 2016 Chris Leishman <chris@leishman.org> - 0.4.0-1
- New upstream release
* Thu Jun 30 2016 Chris Leishman <chris@leishman.org> - 0.3.3-2
- Changed -devel package names to remove soname
* Sat Jun 11 2016 Chris Leishman <chris@leishman.org> - 0.3.3-1
-Initial RPM Release
