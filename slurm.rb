require "formula"


class Slurm < Formula
  homepage "https://github.com/mattthias/slurm"
  url "https://github.com/return/slurm/archive/upstream.zip"
  sha1 "29ec8c911986d4a5c6068f192990d68f74f6596c"


  version "0.4.0"

  depends_on :x11
  depends_on "scons" => :build

  def install

    system "scons"
    bin.install "slurm"

  end

  test do

    system "slurm"
  end
end
