%define name    argus-clients
%define ver     3.0
%if %{?rel:0}%{!?rel:1}
%define rel     6.2
%endif
%if %{?srcext:0}%{!?srcext:1}
%define srcext .gz
%endif
Summary: Argus Client Software
Name: argus-clients
Version: %ver
Release: %rel
License: see COPYING file
Group: Applications/Internet
Source: %{name}-%{version}.%{rel}.tar%{srcext}
URL: http://qosient.com/argus
Buildroot: %{_tmppath}/%{name}-%{ver}-root

%description
Argus Clients contains a number of programs that process Argus data.
Copyright: 2000-2012 QoSient, LLC

%define argusdir        /usr/local
%define argusman        /usr/local/share/man
%define argusdocs       /usr/local/share/doc/%{name}-%{ver}

%define argusbin        %{argusdir}/bin
%define argussbin       %{argusdir}/sbin
%define argusinc        %{argusdir}/include
%define arguslib        %{argusdir}/lib
%define argusdata       %{argusdir}/argus

%prep
%setup -n %{name}-%{ver}.%{rel}
%build
./configure
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR="$RPM_BUILD_ROOT" install

%post

%preun

%postun

%files
%defattr(-,root,root)
%{argusbin}
%{argussbin}
%{argusinc}/argus

%{argusdata}/delegated-ipv4-latest
%{argusdata}/wireshark.manuf.txt
%{argusdata}/rarc

%doc %{argusdocs}

%{arguslib}/argus_client.a
%{arguslib}/argus_common.a
%{arguslib}/argus_parse.a
%{arguslib}/pkgconfig/argus-clients.pc

%{argusman}/man1/ra.1
%{argusman}/man1/rabins.1
%{argusman}/man1/racluster.1
%{argusman}/man1/raconvert.1
%{argusman}/man1/racount.1
%{argusman}/man1/radump.1
%{argusman}/man1/raevent.1
%{argusman}/man1/rafilteraddr.1
%{argusman}/man1/ragraph.1
%{argusman}/man1/ragrep.1
%{argusman}/man1/rahisto.1
%{argusman}/man1/ralabel.1
%{argusman}/man1/ranonymize.1
%{argusman}/man1/rapath.1
%{argusman}/man1/rapolicy.1
%{argusman}/man1/rasort.1
%{argusman}/man1/rasplit.1
%{argusman}/man1/rasql.1
%{argusman}/man1/rasqlinsert.1
%{argusman}/man1/rastream.1
%{argusman}/man1/rastrip.1
%{argusman}/man5/rarc.5
%{argusman}/man5/racluster.5
%{argusman}/man5/ralabel.conf.5
%{argusman}/man5/radium.conf.5
%{argusman}/man8/radium.8
